#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#define BUFFER_SIZE 1024

int main(void)
{
	int character;
	char buf[BUFFER_SIZE];
	while((character=getchar())!='\0')
		putchar(character);

	while((character=getc(stdin))!='\0')
		putc(character,stdout);

	if(ferror(stdin)){
		fprintf(stderr,"standard input error\n");
		exit(1);
	}
	if(fgets(buf,BUFFER_SIZE,stdin)==NULL){
		fprintf(stderr,"fgets error\n");
		exit(1);
	}
	fputs(buf,stdout);
	memset(buf,0,sizeof(buf));
	if(scanf("%s",buf)<0){
		fprintf(stderr,"scanf error\n");
		exit(1);
	}
	printf("%s",buf);
	exit(0);
}
