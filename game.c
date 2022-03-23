#include "game.h"

// FUNÇÕES QUE INICIAM VALORES ALEATÓRIOS
// Número de linhas de defesa aleatório
int get_def_lines_num(){
    return (rand() % (MAXDEFLINE - NSERV + 1)) + NSERV;
}

int get_def_num(int min){
    int lw = min - 1;
    int up = min + (min/2);
    return (rand() % (up - lw + 1)) + lw;
}

void set_random_poke (Pokemon *poke, int id){
    int pid = rand() % NSERV;
    poke->id = id;
    poke->hits = 0;
    poke->pos = 0;
    if (pid == 0){
        strcpy(poke->name, "zubat");
        poke->life = 1;
    } else if (pid == 1){
        strcpy(poke->name, "lugia");
        poke->life = 2;
    } else {
        strcpy(poke->name, "mewtwo");
        poke->life = 3;
    }
}


// FUNÇÃO AUXILIAR
void print_game(Game **game){
    //printf("DefLines: %d \n", (*game)->num_deflines);
    //printf("DefPokes: %d \n", (*game)->num_defs);
    for (int i = 0; i < NSERV+1; i++){
        for (int j = 0; j < (*game)->num_deflines; j++){
            printf("%d ",(*game)->def_line[i][j]);
        }
        printf("\n");
    }
    
    for (int i = 0; i < NSERV; i++){
        printf("%d %s %d %d \n",(*game)->poke[i].id, (*game)->poke[i].name, (*game)->poke[i].life, (*game)->poke[i].hits);
    }   
    printf("\n\n");
}


// INICIA O JOGO
Game *init_game(){
    Game* game = (Game*) malloc(sizeof(Game));
    game->num_poke = 0;

    int i;
    
    for (i = 0; i < NSERV; i++){
        game->num_poke++;
        set_random_poke(&game->poke[i], game->num_poke);
    }
    
    int r = get_def_lines_num();

    for (i = 0; i < NSERV+1; i++){
        for (int j = 0; j < r; j++){
            game->def_line[i][j] = 0;
        }
    }

    game->num_deflines = r;
    game->num_defs = get_def_num(r);
    game->dead_poke = 0;
    game->alive_poke = 0;
    game->turn = 0;

    return game;
}


// FUNÇÕES QUE FAZEM O JOGO ACONTECER
char *get_defenders(Game **game){
    char *rsp = (char*) malloc(BUFSZ);
    char aux[MSGSZ];
    int cont = 0;
    strcpy(rsp, "defender [");
    for (int i = 0; i < NSERV+1; i++){
        for (int j = 0; j < (*game)->num_deflines; j++){
            if ((*game)->def_line[i][j] == 1){
                if (cont == (*game)->num_defs - 1){
                    sprintf(aux, "[%d, %d]]",j+1, i);
                    strcat(rsp, aux);
                } 
                else{
                    sprintf(aux, "[%d, %d], ",j+1, i);
                    strcat(rsp, aux);
                }
                cont++;
            }
        }
    }
    return rsp;
}

void set_defenders(Game **game){
    int i = 0, X, Y;
    while (i < (*game)->num_defs){
        X = rand() % NSERV+1;
        Y = rand() % (*game)->num_deflines;
        if ((*game)->def_line[X][Y] == 0){
            (*game)->def_line[X][Y] = 1;
            i++;
        }
    }
}

char *get_turn(Game **game, int sid, int turn){
    char msg[BUFSZ];
    char *rsp = (char*) malloc(BUFSZ);
    sprintf(msg, "base %d\n", sid);
    strcpy(rsp, msg);

    for (int i = 0; i < (*game)->num_deflines; i++) {
        sprintf(msg, "turn %d\nfixedLocation %d\n", turn, i+1);
        strcat(rsp, msg);
        if ((*game)->poke[sid-1].pos == i) {
            sprintf(msg, "%d %s %d\n", (*game)->poke[sid-1].id, (*game)->poke[sid-1].name, (*game)->poke[sid-1].hits);
            strcat(rsp, msg);
        }
        strcat(rsp, "\n");
    }

    return rsp;
}

// Atualiza todas as variáveis para o novo turno
void set_turn(Game **game, int turn) {
    if (turn != 0 && turn == (*game)->turn + 1){
        // Cada pokemon anda uma posição
        for (int i = 0; i < NSERV; i++){
            (*game)->poke[i].pos++;
            // Pokemon foi morto 
            if ((*game)->poke[i].hits == (*game)->poke[i].life){
                (*game)->num_poke++;
                set_random_poke(&(*game)->poke[i], (*game)->num_poke);
                (*game)->dead_poke++;
            }
            // Pokemon chegou ao fim.
            else if ((*game)->poke[i].pos == (*game)->num_deflines) {
                (*game)->num_poke++;
                set_random_poke(&(*game)->poke[i], (*game)->num_poke);
                (*game)->alive_poke++;
            }
        }
        // Reseta defensores para 1 (caso esteja marcado com 2)
        for (int i = 0; i < NSERV+1; i++){
            for (int j = 0; j < (*game)->num_deflines; j++){
                if ( (*game)->def_line[i][j] == 2) {
                    (*game)->def_line[i][j] = 1;
                }
            }
        }
        (*game)->turn++;
    }
}

int make_shot(Game **game, int X, int Y, int id){
    for (int i = 0; i < NSERV; i++) {
        if ( (*game)->poke[i].id == id ) {
            if ( (*game)->poke[i].pos == X-1 && (i == Y || i == Y-1) && (*game)->def_line[Y][X-1] == 1 ) {
                (*game)->poke[i].hits++;
                (*game)->def_line[Y][X-1] = 2;
                return 1;
            }
            return 0;
        }
    }
    return 0;
}

// 0: todos servidores respondem 
// 1: somente 1 servidor responde
// 2: encerrar
// -1: gameover
int get_action_type (char *action){
    if (strncmp(action, "start", 5) == 0 || strncmp(action, "getturn", 7) == 0){
        return 0;
    } 
    else if (strncmp(action, "getdefenders", 12) == 0 || strncmp(action, "shot", 4) == 0){
        return 1;
    }
    else if (strncmp(action, "quit", 4) == 0){
        return 2;
    } 
    return -1;
}

// Processa as ações e retorna a resposta
char *run_action (char *action, int sid, Game** game){
    char aux[MSGSZ];
    strcpy(aux, action);
    aux [strcspn(aux, "\n")] = 0;
    char *rsp = (char *) malloc(BUFSZ), msg[MSGSZ], *tok = strtok(aux, " ");

    if (strcmp(tok, "start") == 0) {
        sprintf(msg, "game started: path %d", sid);
        strcpy(rsp, msg);
        return rsp;
    }

    else if (strcmp(tok, "getdefenders") == 0) {
        return get_defenders(game);
    }

    else if (strcmp(tok, "getturn") == 0) {
        char *turn = strtok(NULL, " ");
        set_turn(game, atoi(turn));
        return get_turn(game, sid, atoi(turn));
    }

    else if (strcmp(tok, "shot") == 0) {
        char *X = strtok(NULL, " ");
        char *Y = strtok(NULL, " ");
        char *id = strtok(NULL, " ");
        if (make_shot(game, atoi(X), atoi(Y), atoi(id))) {
            sprintf(msg, "shotresp %s %s %s 0", X, Y, id);
        } else {
            sprintf(msg, "shotresp %s %s %s 1", X, Y, id);
        }
        strcpy(rsp, msg);
        return rsp;
    }

    else {
        return "none";
    }
}