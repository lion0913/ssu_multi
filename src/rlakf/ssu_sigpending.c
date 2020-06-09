#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>

int main(void){
	sigset_t set;
	sigset_t pendingset;
	int count=0;

	sigfillset(&set);
	sigprocmask(SIG_BLOCK,&set,NULL);
	while(1){
		printf("count : %d\n",count++);
		sleep(1);
		if(sigpending(&pendingset)==0){
			if(sigismember(&pendingset,SIGINT)){
				printf("SIGINT 블록되어 대기중. 무한루프 종료\n");
				break;
				}
		}
	}
	exit(0);
}
