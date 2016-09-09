#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int prof;
int fd[2],fd2[2], padre;

void crea_procesos(int nivel);
int calc_total(int n);

int main (int argc,char* argv[]){
	pipe(fd);
	pipe(fd2);
	padre = getpid();
	if (argc > 1){
		prof = atoi(argv[1]);
		if(prof < 1) return 0;
		if(prof >= 2)
			crea_procesos(1);			
	}
	int total = calc_total(prof);
	int foo;
	for(int i = 1; i < total; i++)
		read(fd2[0],&foo,sizeof(foo));
	if(padre == getpid()){
		char cmd[50];
		memset(cmd,0,50);
		sprintf(cmd,"pstree %d",padre);
		system(cmd);
		system("killall ntree");
	}
	return 0;
}

void crea_procesos(int nivel){
	int pid = getpid();
	write(fd2[1],&padre,sizeof(padre));
	if (nivel >= prof){
		read(fd[0],&padre,sizeof(padre));
		exit(0);
	}
	for(int i = 0; i <= nivel; i++){
		int id;
		if (!(id=fork()))/*child*/
			crea_procesos(nivel+1);
		if (id == -1){
			fprintf(stderr,"ERROR FORK\n");
			exit(0);
		}
		
	}
	for(int i = 0; i <= nivel; i++){
		if (nivel == 1) break;
		waitpid(-1,NULL,0);
	}
	if(nivel != 1){
		exit(0);
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

