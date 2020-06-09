#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>

void print_file_type(struct stat statbuf){
	char *str;
	if(S_ISREG(statbuf.st_mode))
		str="regular";
	else if(S_ISDIR(statbuf.st_mode))
		str="directory";
	else if(S_ISCHR(statbuf.st_mode))
		str="character";
	else if(S_ISBLK(statbuf.st_mode))
		str="block";
	else if(S_ISFIFO(statbuf.st_mode))
		str="fifo";
	else if(S_ISLNK(statbuf.st_mode))
		str="symlink";
	else if(S_ISSOCK(statbuf.st_mode))
		str="socket";

	printf("%s\n",str);
}
int main(int argc, char *argv[]){
	struct stat statbuf;
	int i;
	if(argc<2){
		fprintf(stderr,"usage: %s <file1> <file2 > ...\n",argv[0]);
		exit(1);
	}
	for(i=1;i<argc;i++){
		if(stat(argv[i],&statbuf)<0){
			fprintf(stderr,"stat error for %s\n",argv[i]);
			exit(1);
		}
		print_file_type(statbuf);
	}
}
