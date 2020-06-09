#include<stdio.h>
#include<stdlib.h>

int main(void){
	FILE *fp;
	char buf[1024];
	if((fp=tmpfile())==NULL){
		fprintf(stderr,"tmpfile error\n");
		exit(1);
	}
	fputs("creat tmpfile successful!!\n",fp);
	fseek(fp,0,SEEK_SET);
	if(fgets(buf,1024,fp)==NULL){
		fprintf(stderr,"fgets error\n");
		exit(1);
	}
	fputs(buf,stdout);
	exit(0);

}
