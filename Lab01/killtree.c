#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct Pid_{
	pid_t pid;
	int nivel;
} pid_;

int prof;
int fd[2], padre;
int fd2[2];
int cant_jovenes;

void imprimir_pids(pid_* proc, int total);
void ordenar(pid_* proc, int n);
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
		pid_ pids[200];
		if(total == 1) return 0;
		for(int i = 0; i < total; i++){
			read(fd2[0],&pids[i],sizeof(pids[i]));
		}
		//imprimir_pids(pids,total);
		ordenar(pids,total);
		//imprimir_pids(pids,total);
		for(int i = 0; i < total - 1; i++){
			char cmd2[50];
			memset(cmd2,0,50);
			sprintf(cmd2,"kill -9 %d",pids[i].pid);
			if(system(cmd2))
				fprintf(stderr,"ERROR con pid: %d de nivel %d\n",pids[i].pid,pids[i].nivel);
			else{
				//fprintf(stderr,"\nPid eliminado: %d de nivel %d\n",pids[i].pid,pids[i].nivel);	
			}
			waitpid(pids[i].pid,NULL,0);
			system(cmd);	
		}
	}
	return 0;
}

void imprimir_pids(pid_* proc, int total){
	fprintf(stderr,"=======================\n");
	for(int i=0;i<total;i++)
		fprintf(stderr, "Nivel: %2d | Pid:%d\n",proc[i].nivel,proc[i].pid);
	fprintf(stderr,"=======================\n");
}

void crea_procesos(int nivel){
	pid_ proc;
	proc.pid = getpid();
	proc.nivel = nivel;
	write(fd2[1],&proc,sizeof(pid_));
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
			//fprintf(stderr, "Nivel %d con pid %d padre %d, Creo proceso %d || Iter: %d\n",nivel,getpid(),getppid(), id,i+1);
		}
	}
	for(int i = 0; i < nivel+1; i++){
		if (nivel == 1) break;
		waitpid(-1,NULL,0);
	}
	if(nivel > 1)
		read(fd[0],&padre,sizeof(padre));
}

void ordenar(pid_* proc, int n){
	for(int i = 0; i < n; i++){
		for(int j = i; j < n;j++){
			if (proc[i].nivel < proc[j].nivel){
				int aux_nivel =proc[i].nivel;
				int aux_pid = proc[i].pid;
				proc[i].nivel =proc[j].nivel;
				proc[i].pid =proc[j].pid;
				proc[j].nivel = aux_nivel;
				proc[j].pid = aux_pid;
			}
		}
	}
}

int calc_total(int n){
	int total = 0;
	int prod = 1;
	for(int i = 1; i <= n; i++){
		prod *= i;
		total += prod;
	}
	return total;
}