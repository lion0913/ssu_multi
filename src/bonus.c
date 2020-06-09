#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>

int main(int argc,char *argv[])
{
	int fd;
	int size;
	char *buf;
	int i;

	if((fd=open("a.txt",O_RDONLY))<0){
		fprintf(stderr," ");
		exit(1);
	}
	size=lseek(fd,0,SEEK_END);
	lseek(fd,0,SEEK_SET);
	buf=(char*)malloc(size);
	read(fd,buf,size);
	for(i=strlen(buf);buf[i]==0;i++){}

	close(fd);

	if((fd=open("a.txt",O_RDWR|O_CREAT|O_TRUNC))<0){
		fprintf(stderr," ");
		exit(1);
	}
	write(fd,buf,strlen(buf));
	write(fd,buf+i,size-i);
	close(fd);
	exit(0);
}

