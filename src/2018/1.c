#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>

void print_file_type(struct stat *statbuf){
	char *str;
	if(S_ISREG(statbuf->st_mode))
		str="regular";
	else if(S_ISDIR(statbuf->st_mode))
		str="directory";
	else if(S_ISCHR(statbuf->st_mode))
		str="character special";
	else if(S_ISBLK(statbuf->st_mode))
		str="block special";
	else if(S_ISFIFO(statbuf->st_mode))
		str="FIFO";
	else if(S_ISLNK(statbuf->st_mode))
		str="symbolic link";
	else if(S_ISSOCK(statbuf->st_mode))
		str="symbolic link";
	else
		str="unknown type";
	printf("%s\n",str);
}

int main(int argc, char *argv[]){
	struct stat statbuf;
	int i;
	FILE *fp;

	if (argc>2){
		fprintf(stderr," ");
		exit(1);
	}
	for (i=1;i<argc;i++){
		if(stat(argv[i],&statbuf)<0){
			fprintf(stderr,"stat error\n");
			exit(1);
		}
		print_file_type(&statbuf);
	}
	exit(0);
}
