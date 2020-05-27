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

#define MAX_VALUE 1000000

int main() {

	int pipeA[2];
	int pipeB[2];

	int res;

	if (pipe(pipeA) == -1) {
		perror("pipe()");

		exit(EXIT_FAILURE);
	}

	if (pipe(pipeB) == -1) {
		perror("pipe()");

		exit(EXIT_FAILURE);
	}

	int contatore = 0;

	switch (fork()) {
		case -1:
			perror("problema con fork");

			exit(EXIT_FAILURE);

		case 0: // processo FIGLIO: legge dalla PIPE

			close(pipeA[1]); // chiudiamo l'estremità di scrittura della pipeA
			close(pipeB[0]); // chiudiamo l'estremità di lettura della pipeB

			while(contatore < MAX_VALUE){
				res = read(pipeA[0], &contatore, sizeof(contatore));
					printf("[child] received %d from pipe\n", contatore);


				if (res == -1) {
					perror("read()");
				}
				contatore++;

				res = write(pipeB[1], &contatore, sizeof(int));
				if (res == -1) {
					perror("write()");
				}
			}


			printf("[child] ho raggiunto il valore massimo, ora termino\n");

			close(pipeA[0]);
			close(pipeB[1]);

			exit(EXIT_SUCCESS);

		default: // processo PADRE: scrive nella PIPE

			printf("[parent] starting\n");

			close(pipeA[0]); // chiudiamo l'estremità di lettura della pipeA
			close(pipeB[1]); // chiudiamo l'estremità di scrittura della pipeB

			res = write(pipeA[1], &contatore, sizeof(int));
			if (res == -1) {
				perror("write()");
			}
			printf("[parent] %d written to pipe\n", res);

			while(contatore < MAX_VALUE){

				res = read(pipeB[0], &contatore, sizeof(contatore));
					printf("[parent] received %d from pipe\n", contatore);


				if (res == -1) {
					perror("read()");
				}
				contatore++;


				res = write(pipeA[1], &contatore, sizeof(int));
				if (res == -1) {
					perror("write()");
				}

			}

			printf("[parent] ho raggiunto il valore massimo, aspetto mio figlio\n");


			wait(NULL);

			printf("[parent] mio figlio ha finito, ora termino\n");

			close(pipeA[1]);
			close(pipeB[0]);

			exit(EXIT_SUCCESS);
	}

	return 0;
}
