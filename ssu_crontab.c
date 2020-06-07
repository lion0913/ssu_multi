#include "ssu_crontab.h"
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // 뮤텍스 객체 선언

int main()
{
	//ssu_crontab_file 있으면 오픈, 없으면 생성
	if(access("ssu_crontab_file",F_OK)<0){
		if(open("ssu_crontab_file",O_RDWR|O_CREAT,0666)<0){
			fprintf(stderr,"open error for ssu_crontab_file\n");
			exit(1);
		}
	}
	//ssu_crontab_log 있으면 오픈, 없으면 생성
	if(access("ssu_crontab_log",F_OK)<0){
		if(open("ssu_crontab_log",O_RDWR|O_CREAT,0666)<0){
			fprintf(stderr,"open error for ssu_crontab_log\n");
			exit(1);
		}
	}
	//프롬프트 함수로 이동
	prompt();
}
void prompt()
{
	FILE *fp;
	int fd;

	while(1)
	{
		char *tmp,*tp;
		char input[BUFFER_SIZE];
		FILE *fp;
		int is_valid=0;
		//그때그때 갱신된 명령어리스트들을 읽어옴
		/*저장된 모든 명령어 출력 및 개행*/
		if((fp=fopen("ssu_crontab_file","r+"))==NULL){
			fprintf(stderr,"fopen error\n");
			exit(1);
		}
		
		cmdnum=0;
		fseek(fp,0,SEEK_SET);
		while(fscanf(fp, "%[^\n]\n", command[cmdnum]) > 0) 
			cmdnum++;
		printf("cmdnum: %d\n",cmdnum);
		for(int i=0;i<cmdnum;i++)
			  printf("%d. %s\n",i,command[i]);
		printf("20182611> ");
		fgets(input,BUFFER_SIZE,stdin);
		tmp=(char *)malloc(BUFFER_SIZE);
		strcpy(tmp,input);

		printf("%s\n",tmp);

		//엔터만 입력시 프롬프트 재출력
		if(strcmp(input,"\n")==0)
			continue;

		//프롬프트 명령어 읽어들이고 유효한지 확인

		token[0]=strtok(input," ");
		int argc=1;

		//printf("%s\n",input);
		while(argc<MAX_TOKEN){
			token[argc]=strtok(NULL," ");
			if(token[argc]==NULL)
			{
				//토큰에 아무값도 들어가지 않은 경우
				for(int i=0;;i++)
				{
					if(token[argc-1][i]=='\n'){
						//토큰의 맨 마지막에 들어온 개행문자를 제거
						token[argc-1][i]='\0';
						break;
					}
				}
				token[argc]="\0";
				break;
			}
			argc++;
		}


		if(strcmp(token[0],"add")==0)
		{
			if(argc<7){//프롬프트 명령어 + 주기[5개] + 명령어[최소 1개]의 최소요건을 갖추지 못한 경우
				fprintf(stderr,"add input error!\n");
				continue;
			}

			//실행주기의 입력이 형식에 맞는지 체크
			if((is_valid=format_check(token))==0)
				continue;

			//command배열에 실행주기를 입력

			sprintf(command[cmdnum],"%s",token[1]);
			for(int i=2;i<argc;i++){
				strcat(command[cmdnum]," ");
				strcat(command[cmdnum],token[i]);
			}
			cmdnum++;
			write_file();
			write_log(command[cmdnum -1],ADD);
			//cmdnum++;
			continue;
		}
		else if(strcmp(token[0],"remove")==0)
		{
			int rmnum;
			char *rmbuf;
			rmbuf=(char *)malloc(BUFFER_SIZE);
			if(argc!=2)
			{
				fprintf(stderr,"wrong input! (remove <COMMAND_NUMBER>)\n");
				continue;
			}
			rmnum=atoi(token[1]);
			if(rmnum < 0 || rmnum > cmdnum){
				fprintf(stderr,"invalid remove number.\n");
				continue;
			}
			strcpy(rmbuf,command[rmnum]);//잘 읽힘
			//printf("rmbuf : %s\n",rmbuf);

			//입력한 번호의 숫자를 crontabfile의 맨 하단으로 이동(제거하기 편하게 하기 위해)

			for(int i=rmnum;i<cmdnum;i++){
				strcpy(tmp,command[i]);
				strcpy(command[i],command[i+1]);
				strcpy(command[i+1],tmp);
			}

			cmdnum--;
			write_file();
			printf("write_file 에 들어갈 리스트들\n");
			for(int i=0;i<cmdnum;i++){
				printf("%s\n",command[i]);
			}
			write_log(rmbuf,REMOVE);


			//등록된 명령어 번호의 범위에 들어있지 않은 경우

			continue;
		}
		else if(strcmp(token[0],"exit")==0)
		{
			exit(0);
		}
		else
		{
			fprintf(stderr,"you can only use add,remove,exit command.\n");
			continue;
		}
	}
	fclose(fp);

}

/*
   실행주기 항목의 형식을 체크하는 함수
   return값 : 1(valid format),0(invalid format)
 */
int format_check(void)
{
	char *tmp;
	char list[MAX_TOKEN][MAX_TOKEN];
	char unit[BUFFER_SIZE];

	memset(list,0,MAX_TOKEN);
	memset(unit,0,BUFFER_SIZE);
	for(int i=1;i<6;i++){
		strcpy(unit,token[i]);

		//각 유닛의 형식 체크(숫자,'*','-',',','/'만 허용)

		//[1] 시작이 기호인 경우 에러처리
		if(unit[0]=='-' || unit[0]=='/' ||unit[0]==',')
		{
			fprintf(stderr," you can't input [%c] at first.\n",unit[0]);
			return 0;
		}
		//[2] 끝이 기호인 경우 에러처리
		if( unit[strlen(unit-1)]=='-' || unit[strlen(unit-1)]==','|| unit[strlen(unit-1)]=='/')
		{
			fprintf(stderr,"you can't input [%c] at last.\n",unit[strlen(unit-1)]);
			return 0;
		}
		//[3] '-', '/', ','부호가 연속되거나 잘못된 값이 들어온 경우 에러처리
		for(int j=0;j<strlen(unit);j++)
		{
			char c=unit[j];
			if( (c>='0' && c <='9')|| (c=='*') || (c=='-') || (c==',') || (c=='/')){}
			else
			{
				fprintf(stderr,"wrong input!\n");
				return 0;
			}
			if(c=='-' || c==',' || c=='/')
			{
				if(unit[j-1]=='-' ||unit[j-1]==','||unit[j-1]=='/')
				{
					fprintf(stderr,"Consecutive symbols are not allowed.\n");
					return 0;
				}
				if(unit[j+1]=='-' ||unit[j+1]==','||unit[j+1]=='/')
				{
					fprintf(stderr,"Consecutive symbols are not allowed.\n");
					return 0;
				}

			}
		}


		int k=0;

		//,인 경우엔 여러개의 숫자가 나오므로 ,를 기준으로 하나씩 분리해서 리스트화해줘야함.
		if(strchr(unit,',')!=NULL)
		{	if((tmp=strtok(unit,","))!=NULL)
			{	
				strcpy(list[k++],tmp);
				while((tmp=strtok(NULL,","))!=NULL)
					strcpy(list[k++],tmp);
			}

			//'/'가 나온 경우 : 숫자 / 숫자의 형태

		}
		else if(strchr(unit,'/')!=NULL)
		{
			if((tmp=strtok(unit,"/"))!=NULL)
			{
				strcpy(list[k++],tmp);
				while((tmp=strtok(NULL,"/"))!=NULL)
					strcpy(list[k++],tmp);
				if(k>2)//3개 이상의 숫자가 들어온 경우 에러처리
				{
					fprintf(stderr,"Use [num] / [num] format. \n");
					return 0;
				}
			}

			//'-'가 나온 경우 : 숫자 - 숫자의 형태
		}
		else if(strchr(unit,'-')!=NULL)
		{
			if((tmp=strtok(unit,"-"))!=NULL)
			{
				strcpy(list[k++],tmp);
				while((tmp=strtok(NULL,"-"))!=NULL)
					strcpy(list[k++],tmp);
				if(k>2)
				{
					fprintf(stderr,"Use [num] - [num] format.\n");
					return 0;
				}
			}
		}
		else
			strcpy(list[k++],unit);

		//각 실행주기별 항목의 값을 벗어난 경우 에러처리
		if(i==MIN)
		{
			for(int a=0;a<k;a++)
				if((list[a][0]!='*')&&(atoi(list[a])<0 || atoi(list[a])>59))
				{
					fprintf(stderr,"MIN is out of range\n");
					return 0;
				}
		}
		else if(i==HOUR)
		{
			for(int a=0;a<k;a++)
				if((list[a][0]!='*')&&(atoi(list[a])<0 || atoi(list[a])>23))
				{
					fprintf(stderr,"HOUR is out of range\n");
					return 0;
				}


		}
		else if(i==DAY)
		{
			for(int a=0;a<k;a++)
				if((list[a][0]!='*')&&(atoi(list[a])<1 || atoi(list[a])>31))
				{
					fprintf(stderr,"DAY is out of range\n");
					return 0;
				}
		}
		else if(i==MON)
		{
			for(int a=0;a<k;a++)
				if((list[a][0]!='*')&&(atoi(list[a])<1 || atoi(list[a])>12))
				{
					fprintf(stderr,"MON is out of range\n");
					return 0;
				}

		}
		else if(i==WEEKDAY)
			for(int a=0;a<k;a++)
				if((list[a][0]!='*')&& (atoi(list[a])<0 || atoi(list[a])>6))
				{
					fprintf(stderr,"WEEKDAY is out of range\n");
					return 0;
				}
	}
	return 1;//형식을 맞춰 제대로 입력된 경우
}
void write_file()
{
	FILE *fp;
	//파일 생성(있으면 생성)
	if((fp=fopen("ssu_crontab_file","w+"))<0)
	{
		fprintf(stderr,"fopen error for ssu_crontab_file\n");
		return;
	}
	for(int i=0;i<cmdnum;i++){			

		fprintf(fp,"%s\n",command[i]);
//		printf("%s\n",command[i]);
	}
	fclose(fp);
}



void write_log(char *input,int stat){
	FILE *fp;
	time_t now;


	pthread_mutex_lock(&mutex);
	if((fp=fopen("ssu_crontab_log","r+"))==NULL){
		if((fp=fopen("ssu_crontab_log","w"))==NULL){
			fprintf(stderr,"ssu_crontab_log open error\n");
			return;
		}
	}	
	fseek(fp,0,SEEK_END);
	time(&now);
	printf("input : %s\n",input);
	switch(stat){
		case ADD:
			fprintf(fp,"[%.24s] %s %s\n",ctime(&now),"add",input);
			break;
		case REMOVE:
			fprintf(fp,"[%.24s] %s %s\n",ctime(&now),"remove",input);
			break;
		case RUN:
			fprintf(fp,"[%.24s] %s %s\n",ctime(&now),"run",input);
			break;
	}
	pthread_mutex_unlock(&mutex);
	fclose(fp);

}
