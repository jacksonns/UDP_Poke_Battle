#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSZ 1024
#define NSERV 4
#define PORTSZ 15
#define TIMEOUT_SECS 2

#define MAXTRIES 5
int tries = 0; 

void CatchAlarm(int ignored) {
	tries += 1;
}

int main(int argc, char **argv) {
	if (argc < 3) {
		exit(EXIT_FAILURE);
	}

	int s;
    struct sockaddr_storage storage;

	if (0 != addrparse(argv[1], argv[2], &storage)) {
		exit(EXIT_FAILURE);
	}

	s = socket(storage.ss_family, SOCK_DGRAM, 0);
	if (s == -1) {
		logexit("socket");
	}

	struct sockaddr *addr = (struct sockaddr *)(&storage);
	socklen_t addrlen = sizeof(storage);

	// Set signal handler for alarm signal
	struct sigaction handler; // Signal handler
	handler.sa_handler = CatchAlarm;
	if (sigfillset(&handler.sa_mask) < 0) // Block everything in handler
		logexit("sigfillset() failed");
	handler.sa_flags = 0;

	//alarm(TIMEOUT_SECS);
	//alarm(0);

	char buf[BUFSZ], aux[BUFSZ];
    size_t count;
	// Manda mensagem inicial de início de jogo
    sprintf(buf, "start\n");
	strcpy(aux,buf);
    sendto(s, buf, strlen(buf), 0, addr, addrlen );
	memset(buf, 0, BUFSZ);
	alarm(TIMEOUT_SECS);
	for (int i = 0; i < 4; i++){
		if (i == 0){
			while ( (count = recvfrom(s, buf, 1024, 0, addr, &addrlen)) < 0 ){
				if (errno == EINTR) { // Alarm went off
					if (tries < MAXTRIES) { // Incremented by signal handler
						count = sendto(s, aux, 1024, 0, addr, addrlen);
					if (count < 0)
						logexit("sendto() failed");
					} else
						logexit("No Response");
				} else
					logexit("recvfrom() failed");
			}
			alarm(0);
			tries = 0;
		} else{
			count = recvfrom(s, buf, 1024, 0, addr, &addrlen );
		}
		printf("< ");
		puts(buf);
	}

	// Inicia comunicação do jogo
	while(1){
		memset(buf, 0, BUFSZ);
		printf("> ");
		fgets(buf, BUFSZ-1, stdin);
		count = sendto(s, buf, strlen(buf)+1, 0, addr, addrlen );

		//memset(buf, 0, BUFSZ);
		strcpy(aux,buf);
		if (strncmp(buf, "getturn", 7) == 0){
			alarm(TIMEOUT_SECS);
			for (int i = 0; i < 4; i++){
				if (i == 0){
					while ( (count = recvfrom(s, buf, 1024, 0, addr, &addrlen)) < 0 ){
					if (errno == EINTR) { // Alarm went off
						if (tries < MAXTRIES) { // Incremented by signal handler
							count = sendto(s, aux, 1024, 0, addr, addrlen);
						if (count < 0)
							logexit("sendto() failed");
						} else
							logexit("No Response");
					} else
						logexit("recvfrom() failed");
					}
					alarm(0);
					tries = 0;
				}else {
					count = recvfrom(s, buf, 1024, 0, addr, &addrlen );
				}
				printf("< ");
				puts(buf);
			}
		} 
		else if (strncmp(buf, "quit", 4) == 0) {
			close(s);
			exit(EXIT_SUCCESS);
		}
		else {
			alarm(TIMEOUT_SECS);
			while ( (count = recvfrom(s, buf, 1024, 0, addr, &addrlen)) < 0 ){
				if (errno == EINTR) { // Alarm went off
					if (tries < MAXTRIES) { // Incremented by signal handler
						count = sendto(s, aux, 1024, 0, addr, addrlen);
					if (count < 0)
						logexit("sendto() failed");
					} else
						logexit("No Response");
				} else
					logexit("recvfrom() failed");
			}
			alarm(0);
			tries = 0;

			printf("< ");
			puts(buf);
		}
	}

	exit(EXIT_SUCCESS);
}