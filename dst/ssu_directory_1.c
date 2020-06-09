#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>

#define DIRECTORY_SIZE 1024

int main(int argc, char *argv[]){
	struct stat statbuf;
	struct dirent *dentry;
	char filename[DIRECTORY_SIZE+1];
	DIR *dirp;

	if((dirp=opendir(argv[1]))==NULL ||chdir(argv[1])==-1){
		fprintf(stderr,"open,chdir error\n");
		exit(1);
	}
	while((dentry=readdir(dirp))!=NULL){
		if(dentry->d_ino=0)
			continue;
		memcpy(filename,dentry->d_name,DIRECTORY_SIZE);
		if(stat(filename,&statbuf)==-1){
			fprintf(stderr,"stat error\n");
			exit(1);
		}
		if((statbuf.st_mode &S_IFMT)==S_IFREG)
			printf("%-14s %ld\n",filename,statbuf.st_size);
		else
			printf("%-14s\n",filename);
	}
	exit(0);
}
