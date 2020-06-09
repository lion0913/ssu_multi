#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>

int main(void){
	sigset_t set;
	int count;
	sigemptyset(&set);
	sigaddset(&set,SIGINT);
	sigprocmask(SIG_BLOCK,&set,NULL);
	for(count=3;0<count;count--){
		printf("count %d\n",count);
		sleep(1);
	}
	printf("Ctrl-C에 대한 블록해제\n");
	sigprocmask(SIG_UNBLOCK,&set,NULL);
	printf("count중 ctrlc눌렀으면 이 문장은 출력안돼\n");
	while(1);
	exit(0);
}
