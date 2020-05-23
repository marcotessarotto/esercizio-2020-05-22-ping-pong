#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void father_process(void);
void child_process(void);

#define MAX_VALUE 1000000

int pipeA[2];
int pipeB[2];
int counter = 0;


int main() {

	if (pipe(pipeA) == -1) {
		perror("pipe()\N");
		exit(1);
	}
	if (pipe(pipeB) == -1) {
		perror("pipe()\N");
		exit(1);
	}

	switch (fork()) {
		case -1:
			perror("fork()\n");
			exit(1);
		case 0:
			child_process();
			exit(0);
		default:
			father_process();
	}

	exit(0);
}

void father_process(void){
	int res;
	printf("proc_padre manda a proc_figlio il valore %d attraverso pipeA.\n", counter);

	while(counter < MAX_VALUE){
		res = write(pipeA[1], &counter, sizeof(counter));
		if(res == -1){
			perror("write()\n");
			exit(1);
		}

		res = read(pipeB[0], &counter, sizeof(counter));
		if(res == -1){
			perror("read()\n");
			exit(1);
		}
		if(counter == MAX_VALUE -1){
			printf("proc_padre riceve il valore %d, lo incrementa (=%d) e finisce\n", counter, (counter+1));
		}else{
			printf("proc_padre riceve il valore %d, lo incrementa (=%d) e lo manda a proc_figlio attraverso pipeA.\n", counter, (counter+1));
		}
		counter++;
	}
}

void child_process(void){
	int res;
	while(counter < MAX_VALUE){
		res = read(pipeA[0], &counter, sizeof(counter));
		if(res == -1){
			perror("read()\n");
			exit(1);
		}
		printf("proc_figlio riceve il valore %d, lo incrementa (=%d) e lo manda a proc_padre attraverso pipeB.\n", counter, (counter+1));
		counter++;

		res = write(pipeB[1], &counter, sizeof(counter));
		if(res == -1){
			perror("write()\n");
			exit(1);
		}
	}
}
