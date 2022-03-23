#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// NÚMERO MÁXIMO DE LINHAS DE DEFESA (MUTÁVEL)
#define MAXDEFLINE 6 

#define BUFSZ 1024
#define MSGSZ 30
#define PORTSZ 15
#define NSERV 4

typedef struct Pokemon{
    char name[10];
    int id;
    int life;
    int hits;
    int pos;  // posição em linha reta
} Pokemon;

typedef struct Game{
    int num_poke;
    int num_deflines;
    int num_defs;
    int dead_poke;
    int alive_poke;
    int turn;
    Pokemon poke[NSERV];
    int def_line[NSERV+1][MAXDEFLINE];
} Game;

// Funções que usam valores aleatórios
int get_def_lines_num();
int get_def_num(int min);
void set_random_poke (Pokemon *poke, int id);

// Função auxiliar para printar o jogo
void print_game(Game **game);

// Inicia o jogo
Game *init_game();

// Funções que fazem o jogo acontecer
char *get_defenders(Game **game);
void set_defenders(Game **game);
char *get_turn(Game **game, int sid, int turn);
void set_turn(Game **game, int turn);
int make_shot(Game **game, int X, int Y, int id);

int get_action_type (char *action);
char *run_action (char *action, int sid, Game** game);
