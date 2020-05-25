/*
 * esercizio-2020-5-22-ping-pong.c
 *
 *Scrivere un programma che realizza un "ping-pong" tra due processi utilizzando una coppia
 *Scrivere di pipe, una per ogni direzione.

Il contatore (di tipo int) viene incrementato ad ogni ping ed ad ogni pong e viene trasmesso
attraverso la pipe.

Quanto il contatore raggiunge il valore MAX_VALUE il programma termina.

proc_padre manda a proc_figlio il valore 0 attraverso pipeA.
proc_figlio riceve il valore 0, lo incrementa (=1) e lo manda a proc_padre attraverso pipeB.
proc_padre riceve il valore 1, lo incrementa (=2) e lo manda a proc_figlio attraverso pipeA.
proc_figlio riceve il valore 2 .....

fino a MAX_VALUE, quando termina il programma.

#define MAX_VALUE 1000000
 *
 *  Created on: May 21, 2020
 *      Author: marco
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#define MAX_VALUE 1000000

int* global_counter;
int pipe_dx[2];
int pipe_sx[2];

int main() {

	int res;
	// pipe direzione PADRE -> FIGLIO
	if (pipe(pipe_dx) == -1) {
		perror("pipe DX()");

		exit(EXIT_FAILURE);
	}

	// pipe direzione FIGLIO -> PADRE
	if (pipe(pipe_sx) == -1) {
			perror("pipe SX()");

			exit(EXIT_FAILURE);
		}

	global_counter = malloc(sizeof(int));
	if(global_counter == NULL){
		perror("malloc()!");
		exit(EXIT_FAILURE);
	}

	*global_counter = 0;

	switch(fork()){
		case -1:

			perror("fork()");
			exit(EXIT_FAILURE);

		case 0:
			// processo figlio
			close(pipe_dx[1]); // estremità scrittura
			close(pipe_sx[0]); // estremità lettura
			//proc_figlio riceve il valore 0, lo incrementa (=1) e lo manda a proc_padre attraverso pipeB.
			while( (res = read(pipe_dx[0],global_counter,sizeof(int)) > 0))  {

				if(*global_counter < MAX_VALUE){
					(*global_counter)++;
					printf("[child] global_counter incrementato %d \n", *global_counter);
					res = write(pipe_sx[1],global_counter,sizeof(int));

					if( res == -1){
						perror("Write in pipe! ");
						exit(EXIT_FAILURE);
					}
				} else {
					printf("[child] MAX_VALUE raggiunto!\n");

					res = write(pipe_sx[1],global_counter,sizeof(int));

					if( res == -1){
						perror("Write in pipe! ");
						exit(EXIT_FAILURE);
					}

					exit(EXIT_SUCCESS);
				}
			}
			if (res == 0){
				perror("[child] pipe closed\n");
				exit(EXIT_FAILURE);
			}

			exit(EXIT_SUCCESS);

		default:
			// processo padre
			close(pipe_dx[0]);
			close(pipe_sx[1]);

			// il proc_padre manda al proc-figlio il valore 0 attraverso pipeA
			if (write(pipe_dx[1],global_counter,sizeof(int)) == -1){
				perror("Write in pipe!");
				exit(EXIT_FAILURE);
			}

			while( (res = read(pipe_sx[0],global_counter,sizeof(int)) > 0)) {
				if(*global_counter < MAX_VALUE){
					(*global_counter)++;
					printf("[parent] global_counter incrementato %d \n", *global_counter);
					res = write(pipe_dx[1],global_counter,sizeof(int));

					if( res == -1){
						perror("Write in pipe! ");
						exit(EXIT_FAILURE);
					}
				} else {
						printf("[parent] MAX_VALUE raggiunto!\n");
						break;
				}
			}

			if (res == 0){
				perror("[parent] pipe closed\n");
				exit(EXIT_FAILURE);
			}

			break;

	}

	res = wait(NULL);
	if(res == -1){
		perror("wait!");
		exit(EXIT_FAILURE);
	}

	printf("[parent] global_counter %d \n",*global_counter);
	printf("bye!\n");
	exit(EXIT_SUCCESS);
}
