#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(void)
{
	pid_t pid;
	char character,first,last;
	long i;
	if((pid=fork())<0){
		fprintf(stderr,"fork error\n");
		exit(1);
	}
	else if(pid==0){
		first='a';
		last='z';
	}
	else{
		first='A';
		last='Z';
	}
	for(character=first;character<=last;character++){
		write(1,&character,1);
	}
	printf("\n");
	exit(0);
}
