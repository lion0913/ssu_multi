#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>

int main(int argc, char *argv[])
{
	int fd;
	int size;
	char *buf;
	int i;
	if((fd=open("a.txt",O_RDONLY))<0){
		fprintf(stderr,"open error\n");
		exit(1);
	}
	size=lseek(fd,0,SEEK_END);
	//size=ftell(fd);
	printf("%d\n",size);
	close(fd);
	exit(0);
}

