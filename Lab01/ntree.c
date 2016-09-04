#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int prof;
int fd[2], padre;

void crea_procesos(int nivel);

int main (int argc,char* argv[]){
	pipe(fd);
	padre = getpid();
	if (argc > 1){
		prof = atoi(argv[1]);
		if(prof < 1) return 0;
		fprintf(stderr,"%d\n",prof);
		if(prof >= 2)
			crea_procesos(1);			
	}	
	if(padre == getpid()){
		char cmd[50];
		memset(cmd,0,50);
		sprintf(cmd,"pstree %d",padre);
		system(cmd);
		exit(0);
	}
	return 0;
}

void crea_procesos(int nivel){
	int pid = getpid();
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
			fprintf(stderr, "Nivel %d con pid %d padre %d, Creo proceso %d || Iter: %d\n",nivel,getpid(),getppid(), id,i+1);
		}
	}
	for(int i = 0; i < nivel+1; i++){
		if (nivel == 1) break;
		waitpid(-1,NULL,0);
	}
	if(nivel > 1)
		read(fd[0],&padre,sizeof(padre));
}

