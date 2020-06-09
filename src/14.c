#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

#define BUFFER_SIZE 128

int main(int argc,char *argv[])
{
	char buf[BUFFER_SIZE];
	int fd1,fd2;
	ssize_t size;

	if((fd1=open(argv[1],O_RDONLY|O_CREAT,0644))<0){
		fprintf(stderr,"open error \n");
		exit(1);
	}
	size=read(fd1,buf,BUFFER_SIZE);
	close(fd1);

	if((fd2=open(argv[2],O_RDWR|O_CREAT|O_TRUNC,0644))<0){
		fprintf(stderr,"open error 2\n");
		exit(1);
	}
	write(fd2,buf,size);
	close(fd2);
	exit(0);
}

