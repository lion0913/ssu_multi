#include<stdio.h>
#include<stdlib.h>
#include<signal.h>

int main(void){
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set,SIGINT);
	switch(sigismember(&set,SIGINT)){
		case 1:
			printf("included\n");
			break;
		case 0:
			printf("not included\n");
			break;
		default:
			printf("fail\n");
		}
	switch(sigismember(&set,SIGSYS)){
		case 1:
			printf("included\n");
			break;
		case 0:
			printf("not included\n");
			break;
		default:
			printf("fail\n");
		}
	exit(0);
}
