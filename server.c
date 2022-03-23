#include "common.h"
#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

#include <sys/socket.h>
#include <sys/types.h>


int main(int argc, char **argv) {
    if (argc < 3) {
        exit(EXIT_FAILURE);
    }

    struct sockaddr_storage storage;

    if ( 0 != server_sockaddr_init(argv[1], argv[2], &storage) ) {
        exit(EXIT_FAILURE);
    }

    int s = socket(storage.ss_family, SOCK_DGRAM, 0);
    if ( s == -1 ) {
        logexit("socket");
    }

    int enable = 1;
    if ( 0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) ) {
        logexit("setsockopt");
    }
    
    struct sockaddr *addr = (struct sockaddr *)(&storage);

    if ( 0 != bind(s, addr, sizeof(storage)) ) {
        logexit("bind");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", addrstr);


    struct sockaddr_storage cstorage;
    struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
    socklen_t caddrlen = sizeof(cstorage);

    srand(time(0));

    size_t count;
    int type;

    while (1){
        Game *game = init_game();
        set_defenders(&game);

        char buf[BUFSZ], *rsp;
        time_t begin, end;
        int flag = 1;
        while (1) {
            memset(buf, 0, BUFSZ);
            count = recvfrom(s, buf, 1024, 0, caddr, &caddrlen);
            if (count == 0 || count == -1){
                close(s);
                break;
            }
            
            printf("action: %s", buf);
            type = get_action_type(buf);
            int i = 1;
            if (type == 0){
                // Todos servidores respondem
                if (flag == 1) {
                    begin = time(NULL);
                    flag = 0;
                }
                for (i = 1; i <= NSERV; i++){
                    rsp = run_action(buf, i, &game);
                    if (i == 1) print_game(&game);
                    sendto(s, rsp, strlen(rsp), 0, caddr, caddrlen);
                }
            }
            else if (type == 1){
                // 1 servidor responde
                rsp = run_action(buf, 1, &game);
                print_game(&game);
                sendto(s, rsp, strlen(rsp), 0, caddr, caddrlen);
            }
            else if (type == 2){
                // quit
                free(game);
                close(s);
                exit(EXIT_SUCCESS);
            }
            else {
                // gameover
                end = time(NULL);
                char aux[MSGSZ];
                sprintf(aux, "gameover %d %d %ld", game->dead_poke, game->alive_poke, (end - begin));
                strcpy(rsp, aux);
                sendto(s, rsp, strlen(rsp), 0, caddr, caddrlen);
                break;
            }

        }
    }

    exit(EXIT_SUCCESS);
}