#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int prof;
int fd[2], padre;
int fd2[2];
int cant_jovenes;

void ordenar(int* pids, int n);
void crea_procesos(int nivel);
int calc_total(int n);

int main (int argc,char* argv[]){
	pipe(fd);
	pipe(fd2);
	padre = getpid();
	if (argc > 1){
		prof = atoi(argv[1]);
		if (prof < 1) return 0;
		fprintf(stderr,"Niveles: %d\n",prof);
		cant_jovenes = prof * (prof -1);
		if (prof >= 2)
			crea_procesos(1);			
	}	
	if(padre == getpid()){
		char cmd[50];
		memset(cmd,0,50);
		sprintf(cmd,"pstree %d",padre);
		system(cmd);
		int total = calc_total(prof);
		int pids[200];
		if(total == 1) return 0;
		for(int i = 0; i < total; i++){
			read(fd2[0],&pids[i],sizeof(pids[i]));
		}
		ordenar(pids,total);
		for(int i = 0; i < total - 1; i++){
			char cmd2[50];
			memset(cmd2,0,50);
			sprintf(cmd2,"kill -9 %d",pids[i]);
			system(cmd2);
			waitpid(pids[i],NULL,0);
			system(cmd);	
		}
	}
	return 0;
}

void crea_procesos(int nivel){
	int pid = getpid();
	write(fd2[1],&pid,sizeof(pid));
	if (nivel >= prof){
		read(fd[0],&padre,sizeof(padre));
		exit(0);
		return;
	}
	for(int i = 0; i < nivel+1; i++){
		int id;
		if (!(id=fork())){/*child*/
			crea_procesos(nivel+1);
			break;
		} else {
			fprintf(stderr, "Nivel %d con pid %d padre %d, Creo proceso %d || Iter: %d\n",nivel+1,getpid(),getppid(), id,i+1);
		}
	}
	for(int i = 0; i < nivel+1; i++){
		if (nivel == 1) break;
		waitpid(-1,NULL,0);
	}
}

void ordenar(int* pids, int n){
	for(int i = 0; i < n; i++){
		for(int j = i; j < n;j++){
			if (pids[i] < pids[j]){
				int aux = pids[i];
				pids[i] = pids[j];
				pids[j] = aux;
			}
		}
	}
}

int calc_total(int n){
	int total = 0;
	while(n > 1){
		total = total + (n*(n-1));
		n = n -1;
	}
	return total + 1;
}
