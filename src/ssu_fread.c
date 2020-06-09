#include<stdio.h>
#include<stdlib.h>

struct ssu_pirate{
	unsigned long booty;
	unsigned int beard_length;
	char name[128];
};
int main(void)
{
	struct ssu_pirate blackbeard={950,48,"Edward Teach"},pirate;
	char *fname="ssu_data";
	FILE *fp1,*fp2;
	if((fp2=fopen(fname,"w"))==NULL){
		fprintf(stderr,"fopen error for %s\n",fname);
		exit(1);
	}
	if(fread(&blackbeard,sizeof(struct ssu_pirate),1,fp2)!=1){
		fprintf(stderr,"fwrite error\n");
		exit(1);
	}
	fclose(fp2);
	fp1=fopen(fname,"w");
	if(fread(&blackbeard,sizeof(struct ssu_pirate),1,fp1)!=1){
		fprintf(stderr,"fread error\n");
		exit(1);
	}
	
	fclose(fp1);
	exit(0);

