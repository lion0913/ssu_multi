#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<sys/time.h>

#define SEC_TO_MICRO 1000000
int main(int argc,char *argv[]){
	struct timeval begin_t,end_t;
	if(argc<2){
		fprintf(stderr,"usage: <file1> <file2>\n");
		exit(1);
	}
	gettimeofday(&begin_t,NULL);
	if(link(argv[1],argv[2])==-1){
		fprintf(stderr,"link error\n");
		exit(1);
	}
	if(unlink(argv[1])==-1){
		fprintf(stderr,"unlink error\n");
		exit(1);
	}
	gettimeofday(&end_t,NULL);

	if(begin_t.tv_usec>end_t.tv_usec){
		end_t.tv_usec=end_t.tv_usec+SEC_TO_MICRO-begin_t.tv_usec;
		end_t.tv_sec=end_t.tv_sec-(1+begin_t.tv_sec);
	}
	else{
		end_t.tv_usec=end_t.tv_usec-begin_t.tv_usec;
		end_t.tv_sec=end_t.tv_sec-begin_t.tv_sec;
	}
	end_t.tv_usec=begin_t.tv_usec/1000;
	printf("Runtime : %ld %ld\n",end_t.tv_sec,end_t.tv_usec);


	exit(0);
}
/*
 * <sys/time.h>
 * gettimeofday(&begin_t,NULL);
 * gettimeofday(&end_t,NULL);
 * if(begin_t.tv_usec>end_t.tv_usec){
 * end_t.tv_usec=end_t.tv_usec+SEC_TO_MICRO-begin_t.tv_usec;
 * end_t.tv_sec=end_t.tv_sec-(1+begin_t.tv_sec);
 * }else{
 * end_t.tv_usec=end_t.tv_usec-begin_t.tv_usec;
 * end_t.tv_sec=end_t.tv_sec-begin_t.tv_sec;
 * }
 * end_t.tv_usec=begin_t.tv_usec/1000;
 * */
