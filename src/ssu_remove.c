#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	if(argc!=3){
		fprintf(stderr,"usage : <file1>\n");
		exit(1);
	}
	if(link(argv[1],argv[2])<0){
		fprintf(stderr,"link error\n");
		exit(1);
	}
	if(remove(argv[1])<0){
		fprintf(stderr,"remove error for %s\n",argv[1]);
		remove(argv[2]);
		exit(1);
	}
	else
		printf("passed\n");
	printf("Success\n");
	exit(1);
}


