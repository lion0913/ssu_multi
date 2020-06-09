#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(int argc, char *argv[]){
	pid_t pid;
	char character,first,last;
	long i;

	if((pid=fork())<0){
		fprintf(stderr," ");
		exit(1);
	}
	else if(pid==0){
		first='A';
		last='Z';
	}
	else{
		first='a';
		last='z';
	}
	
	for(character=first;character<=last;character++){
		for(i=0;i<=100000;i++){}
		//fputc(character,stdout);
		write(1,&character,1);
	}
	exit(0);
}
