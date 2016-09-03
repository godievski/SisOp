#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


int prof;
int fd[2], padre;

int main(int argc, char* argv[]){
	int pids[30];
	padre = getpid();
	fprintf(stderr,"Ancestro :D %d\n",padre);
	if(argc > 1){
		prof = atoi(argv[1]);
		if (prof >= 30 && prof < 1) exit(1);
		pipe(fd);	
		for(int i = 0; i < prof - 1; i++){
			if(!fork()){/*child*/
				pid_t pid = getpid();
				write(fd[1],&pid,sizeof(pid));
			} else break;
		}
		
		if(padre == getpid()){
			pids[0] = padre;
			for(int i = 1; i < prof; i++){
				read(fd[0],&pids[i],sizeof(pids[i]));
			}
			ordenar(pids,prof);
			for(int i = 0; i < prof; i++){
				fprintf(stderr,"pid: %d\n",pids[i]);
			}
		}
	}
	
	return 0;
}

void ordenar(int* pids, int n){
	for(int i = 0; i < n; i++){
		for(int j = i; j < n;j++){
			if (pids[i] > pids[j]){
				int aux = pids[i];
				pids[i] = pids[j];
				pids[j] = aux;
			}
		}
	}
}

