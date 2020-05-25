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
Scrivere un programma che realizza un "ping-pong" tra due processi utilizzando una coppia di pipe,
una per ogni direzione.

Il contatore (di tipo int) viene incrementato ad ogni ping ed ad ogni pong e viene trasmesso attraverso la pipe.

Quanto il contatore raggiunge il valore MAX_VALUE il programma termina.

proc_padre manda a proc_figlio il valore 0 attraverso pipeA.
proc_figlio riceve il valore 0, lo incrementa (=1) e lo manda a proc_padre attraverso pipeB.
proc_padre riceve il valore 1, lo incrementa (=2) e lo manda a proc_figlio attraverso pipeA.
proc_figlio riceve il valore 2 .....

fino a MAX_VALUE, quando termina il programma.
(il primo processo che arriva a MAX_VALUE fa terminare il programma).


 */

#define MAX_VALUE 1000000

#define CHECK_ERR(a,msg) {if ((a) == -1) { perror((msg)); exit(EXIT_FAILURE); } }

int pipeA[2];
int pipeB[2];
// proc_padre legge da pipeB, scrive su pipeA
// proc_figlio legge da pipeA, scrive su pipeB

int main() {

	int res;
	int counter = 0;

	res = pipe(pipeA);
	CHECK_ERR(res, "pipe")

	res = pipe(pipeB);
	CHECK_ERR(res, "pipe")

	switch(fork()) {
		case 0: // child process

			// proc_figlio legge da pipeA, scrive su pipeB
			close(pipeA[1]);
			close(pipeB[0]);

			while (counter < MAX_VALUE) {

				res = read(pipeA[0], &counter, sizeof(counter));
				CHECK_ERR(res,"read")

				if (res == 0) {
					printf("[child] EOF su pipeA\n");
					break;
				}

				printf("[child] dopo read: counter=%d\n", counter);

				counter++;

				printf("[child] prima di write: counter=%d\n", counter);

				res = write(pipeB[1], &counter, sizeof(counter));
				CHECK_ERR(res,"write")
			}

			printf("[child] fuori da while: counter=%d\n", counter);

			printf("[child] bye\n");

			exit(EXIT_SUCCESS);
		case -1:
			perror("fork()");
			exit(EXIT_FAILURE);
		default:
			;
	}

	// proc_padre legge da pipeB, scrive su pipeA
	close(pipeA[0]);
	close(pipeB[1]);

	while (counter < MAX_VALUE) {

		printf("[parent] prima di write: counter=%d\n", counter);

		res = write(pipeA[1], &counter, sizeof(counter));
		CHECK_ERR(res,"write")

		res = read(pipeB[0], &counter, sizeof(counter));
		CHECK_ERR(res,"read")

		if (res == 0) {
			printf("[parent] EOF su pipeA\n");
			break;
		}

		printf("[parent] dopo read: counter=%d\n", counter);

		counter++;
	}

	printf("[parent] fuori da while: counter=%d\n", counter);

	printf("[parent] bye\n");


	return 0;
}
