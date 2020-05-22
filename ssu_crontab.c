#include "ssu_crontab.h"

int main()
{
	prompt();
}
void prompt()
{
	FILE *fp;
	int fd;
	//if(access("ssu_crontab_file",F_OK)<0)
	//{
	if((fd=open("ssu_crontab_file",O_WRONLY|O_CREAT,0666))<0){
		fprintf(stderr,"open error for ssu_crontab_file\n");
		exit(1);
	}
	
	while(1)
	{
		char input[BUFFER_SIZE];
		char *token[MAX_TOKEN];
		int is_valid=0;
		//그때그때 갱신된 명령어리스트들을 읽어옴
		//get_commandlist();

		/*"ssu_crontab_file"에 저장된 모든 명령어 출력 및 개행*/
		//printcrontab();
		printf("20182611> ");
		fgets(input,sizeof(input),stdin);

		//엔터만 입력시 프롬프트 재출력
		if(strcmp(input,"\n")==0)
			continue;
		
		//프롬프트 명령어 읽어들이고 유효한지 확인

		token[0]=strtok(input," ");
		int argc=1;
		
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
			
			//실행주기의 입력이 올바르게 되었는지 체크
			if(is_valid=format_check(token)==0)
				continue;
			
			
			continue;
		}
		else if(strcmp(token[0],"remove")==0)
		{
			printf("remove!\n");
			if(argc!=2)
			{
				fprintf(stderr,"wrong input! (remove <COMMAND_NUMBER>)\n");
				continue;
			}
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

}

/*
   실행주기 항목의 형식을 체크하는 함수
   return값 : 1(valid format),0(invalid format)
 */
int format_check(char **token)
{
	char *tmp;
	char list[MAX_TOKEN][MAX_TOKEN];
	char unit[BUFFER_SIZE];

	dd memset(list,0,MAX_TOKEN);
	memset(unit,0,BUFFER_SIZE);
	for(int i=1;i<6;i++){
		//printf("%d : %s\n",i,token[i]);
		strcpy(unit,token[i]);
		//printf("unit[strlen] : %c\n",unit[strlen(unit)-1]);
		/*printf("unit : %s,%ld\n",unit,strlen(unit));
		printf("unit[strlen] : %c\n",unit[strlen(unit)]);
		
		if(strtok(unit,"/")){
			printf("tmp 없엉");
		}
		printf("/앞의 값 : %s\n,tmp : %s\n",unit,tmp);
	*/
		
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
		if((tmp=strtok(unit,","))!=NULL)
		{	
			strcpy(list[k++],tmp);
			while((tmp=strtok(NULL,","))!=NULL)
				strcpy(list[k++],tmp);
		}
		else if((tmp,strtok(unit,"/"))!=NULL)
		{
			//'/'가 나온 경우엔 숫자 / 숫자의 형태에서 벗어날 수 없음. :
			
			strcpy(list[k++],tmp);
			tmp
		/*for(int a=0;a<k;a++)
			printf("list [%d] : %s\n",a,list[a]);
		*/

	}
	return 1;//형식을 맞춰 제대로 입력된 경우
}
/*
int make_tokens(int argc, char *argv[],char *input[])
{

}*/
