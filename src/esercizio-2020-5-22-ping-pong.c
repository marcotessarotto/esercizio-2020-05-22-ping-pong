#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_VALUE 1000000


int main() {

	int pipeA[2];
	int pipeB[2];
	int counter=0;

	if (pipe(pipeA) == -1) {
		perror("problema con pipe");
		exit(EXIT_FAILURE);
	}

	if (pipe(pipeB) == -1) {
		perror("problema con pipe");
		exit(EXIT_FAILURE);
	}

	switch (fork()) {
		case -1:
			perror("problema con fork");

			exit(EXIT_FAILURE);

		case 0:{ // processo FIGLIO

			//dal padre deve solo leggere
			if( (close(pipeA[1])) == -1){
				perror("close");
				exit(EXIT_FAILURE);
			}

			//al padre deve solo inviare
			if( (close(pipeB[0])) == -1){
				perror("close");
				exit(EXIT_FAILURE);
			}

			while(counter<(MAX_VALUE)){
				if ((read(pipeA[0], &counter, sizeof(int))) == -1) {
					perror("read");
					exit(EXIT_FAILURE);
				}

				counter++;

				if ((write(pipeB[1], &counter, sizeof(int))) == -1) {
					perror("write");
					exit(EXIT_FAILURE);
				}
			}

			//rilascio le risorse
			if( (close(pipeA[0])) == -1){
				perror("close");
				exit(EXIT_FAILURE);
			}

			if( (close(pipeB[1])) == -1){
				perror("close");
				exit(EXIT_FAILURE);
			}

			printf("[child]: ho finito il while, counter vale: %d\n", counter);
			exit(EXIT_SUCCESS);
			}

		default:{// processo PADRE:

			//al figlio deve solo inviare
			if( (close(pipeA[0])) == -1){
				perror("close");
				exit(EXIT_FAILURE);
			}

			//dal figlio deve solo leggere
			if( (close(pipeB[1])) == -1){
				perror("close");
				exit(EXIT_FAILURE);
			}

			while(counter<MAX_VALUE){

				if ( (write(pipeA[1], &counter, sizeof(int))) == -1){
					perror("close");
					exit(EXIT_FAILURE);
				}

				if ( (read(pipeB[0], &counter, sizeof(int))) == -1){
					perror("close");
					exit(EXIT_FAILURE);
				}

				counter++;

			}

			printf("[parent]: ho finito il while, ora aspetto il figlio\n");

			if (wait(NULL)==-1){
				perror("wait");
				exit(EXIT_FAILURE);
			}

			//rilascio le risorse
			if( (close(pipeA[1])) == -1){
				perror("close");
				exit(EXIT_FAILURE);
			}

			if( (close(pipeB[0])) == -1){
				perror("close");
				exit(EXIT_FAILURE);
			}

			printf("[parent]: ho finito il while, counter vale: %d\n", counter);
			exit(EXIT_SUCCESS);
		}
	}
}
























