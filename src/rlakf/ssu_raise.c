#include<stdio.h>
#include<stdlib.h>
#include<signal.h>


void ssu_signal_handler1(int signo);
void ssu_signal_handler2(int signo);

int main(void){
	if(signal(SIGINT,ssu_signal_handler1)==SIG_ERR){
		fprintf(stderr,"cannot handle SIGINT\n");
		exit(1);
	}
	if(signal(SIGUSR1,ssu_signal_handler2)==SIG_ERR){
		fprintf(stderr," ");
		exit(1);
	}

	raise(SIGINT);
	raise(SIGUSR1);
	printf("main return\n");
	exit(0);
}

void ssu_signal_handler1(int signo){
	printf("sigint발생\n");
}
void ssu_signal_handler2(int signo){
	printf("sigusr1발생\n");
}
