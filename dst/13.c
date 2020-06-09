#include<stdio.h>
#include<stdlib.h>

typedef struct _person{
	char name[10];
	int age;
	double height;
}Person;

int main(void){
	FILE *fp;
	int i,res;
	int size;//double height;
	Person ary[3]={{"Hong Gd",500,175.4},
		{"Lee SS",350,180.0},
		{"King SJ",500,178.6}};
	Person tmp;
	size=sizeof(ary)/sizeof(ary[0]);

	if((fp=fopen("ftest.txt","w"))<0){
		fprintf(stderr,"fopen error\n");
		exit(1);
	}
	for(i=0;i<size;i++){
		if(fwrite(&ary[i],sizeof(Person),1,fp)!=1)
			break;
	}
	fclose(fp);
	if((fp=fopen("ftest.txt","r"))<0){
		fprintf(stderr,"fopen error for \n");
		exit(1);
	}

	printf("[First print]\n");
	while(!feof(fp)){
		if(fread(&tmp,sizeof(Person),1,fp)!=1)
			break;
		printf("%s %d %2.lf\n",tmp.name,tmp.age,tmp.height);
	}
	rewind(fp);
	printf("[Second print]\n");
	while(!feof(fp)){
		if(fread(&tmp,sizeof(Person),1,fp)!=1)
			break;
		printf("%s %d %2.lf\n",tmp.name,tmp.age,tmp.height);
	}
	exit(0);
}
