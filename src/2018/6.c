#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define BUFFER_SIZE 256
#define STUDENT_NUM 3
#define Q_SIZE 3

typedef struct _student{
	char name[10];
	int score;
	char res[BUFFER_SIZE];
}Student;

char answer[BUFFER_SIZE]="123";

int main(void){
	char *ssu_answer="ssu_answer.txt";
	char *ssu_res="ssu_res.txt";
	char tmp_score[BUFFER_SIZE];
	FILE *fp;
	int i,j=0;
	Student list[STUDENT_NUM];

	if((fp=fopen(ssu_answer,"r"))<0){
		fprintf(stderr,"fopen error\n");
		exit(1);
	}
	for(j=0;j<STUDENT_NUM;j++){
		list[j].score=0;
		if(fgets(list[j].name,BUFFER_SIZE,fp)==NULL){
			fprintf(stderr," " );
			exit(1);
		}
		i=0;
		while(list[j].name[i]=='\n') i++;
		list[j].name[i]='\0';
		
		if(fgets(list[j].res,BUFFER_SIZE,fp)==NULL){
			fprintf(stderr," ");
			exit(1);
		}
		i=0;
		for(i=0;i<Q_SIZE;i++){
			if(list[j].res[i]==answer[i]){
				list[j].score+=10;
				list[j].res[i]='O';
			}
			else
				list[j].res[i]='X';
		}
		//printf("%s %d %s",,,);



	for(j=0;j<STUDENT_NUM;j++){
		list[j].score=0;
		if(fgets(list[j].name,BUFFER_SIZE,fp)==NULL){
			fprintf(stderr,"fgets error\n");
			exit(1);
		}
		i=0;
		while((list[j].name[i]!='\n'))i++;
		list[j].name[i]='\0';

		if(fgets(list[j].res,BUFFER_SIZE,fp)==NULL){
			fprintf(stderr,"fgets error\n");
			exit(1);
		}
		i=0;
		while(list[j].res[i]!='\n')i++;
		list[j].res[i]='\0';
		for(i=0;i<Q_SIZE;i++){
			if(answer[i]==list[j].res[i]){
				list[j].score+=10;
				list[j].res[i]='O';
			}
			else
				list[j].res[i]='X';
		}
		printf("Student name : %s , score : %d , res : %s\n",list[j].name,list[j].score,list[j].res);
	}
	fclose(fp);
	if((fp=fopen(ssu_res,"w"))<0){
		fprintf(stderr,"fopen error\n");
		exit(1);
	}
	for(j=0;j<STUDENT_NUM;j++){
		fprintf(fp,"%s | %d | %s\n",list[j].name,list[j].score,list[j].res);
	}
	fclose(fp);
	exit(0);

	

}
