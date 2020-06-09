#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

void main(){
	char *fname="ssu_line.txt";
	char *frname="ssu_blank.txt";
	int fd_w,fd_r;
	size_t length;
	int wordcnt=1;
	char buf[50];
	int i=0;
	if((fd_w=open(fname,O_WRONLY|O_CREAT))<0){
		fprintf(stderr,"creat error for %s\n",fname);
		exit(1);
	}
	if((length=read(buf,1024,fd_w))<0){
		fprintf(stderr,"read error\n");
		exit(1);
	}
	if((fd_r=open(fname,O_RDONLY|O_CREAT))<0){
		fprintf(stderr,"open error for %s\n");
		exit(1);
	}
}
