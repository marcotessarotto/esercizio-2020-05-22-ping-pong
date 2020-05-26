#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <errno.h>


/*
 * esempio:
 *
 * processo padre prepara una pipe e la usa per inviare al processo figlio dei dati
 *
 */

#define BUFFER_SIZE 8
#define MAX_VALUE 1000000


int main(int argc, char * argv[]) {

		int pipeA[2];
		int pipeB[2];
		int res;
		int res1;
		long * buffer;

		if (pipe(pipeA) == -1) {
			perror("pipe()");

			exit(EXIT_FAILURE);
		}


		if (pipe(pipeB) == -1) {
			perror("pipe()");

			exit(EXIT_FAILURE);
		}

		// pipe_fd[0] : estremità di lettura della pipe
		// pipe_fd[1] : estremità di scrittura della pipe

		switch (fork()) {
			case -1:
				perror("problema con fork");

				exit(EXIT_FAILURE);

			case 0: // processo FIGLIO: legge dalla PIPE

				close(pipeA[1]); // chiudiamo l'estremità di scrittura della pipeA
				close(pipeB[0]); // chiudiamo l'estremità di lettura della pipeB

				buffer = malloc(BUFFER_SIZE);
				if (buffer == NULL) {
					perror("malloc()");
					exit(EXIT_FAILURE);
				}

				while(*buffer< MAX_VALUE)
				{

				res = read(pipeA[0], buffer, BUFFER_SIZE);

				if (res == -1) {
					perror("read()");
				}

				*buffer=*buffer+1;

				printf("PONG\n");
				res1 = write(pipeB[1], buffer, BUFFER_SIZE);
								if (res == -1) {
									perror("write()");
				}



				if (res1 == -1) {
									perror("read()");
									}
				}

				close(pipeA[0]);
				close(pipeB[1]);

				exit(EXIT_SUCCESS);

			default: // processo PADRE: scrive nella PIPE



				close(pipeA[0]); // chiudiamo l'estremità di lettura della pipe
				close(pipeB[1]); // chiudiamo l'estremità di scrittura della pipe


				buffer = malloc(BUFFER_SIZE);
				*buffer = 0;

				while(*buffer < MAX_VALUE)
				{
				// se pipe piena (capacità: 16 pages) allora write() si blocca
				printf("PING %ld\n",*buffer);
				res = write(pipeA[1], buffer, BUFFER_SIZE);
				if (res == -1) {
					perror("write()");
				}



				res = read(pipeB[0], buffer, BUFFER_SIZE);

								if (res == -1) {
									perror("read()");
								}


				}








				wait(NULL);

				close(pipeA[1]); // chiudiamo estremità di scrittura della pipe
				close(pipeB[0]);

				exit(EXIT_SUCCESS);
		}



}
