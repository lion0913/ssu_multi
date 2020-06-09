#include "ssu_crond.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int main(void){
	pthread_t tid[BUFFER_SIZE];
	struct stat statbuf;
	FILE *fp;
	struct timeval begin,end;
	time_t now;

	gettimeofday(&begin,NULL);

	//크론탭파일은 읽기권한으로 오픈
	if(access("ssu_crontab_file",F_OK)<0){
		if((fp=fopen("ssu_crontab_file","w"))<0){
			fprintf(stderr,"open error for ssu_crontab_file\n");
			exit(1);
		}
	} else fp = fopen("ssu_crontab_file", "r");


	//로그파일은 작성할 것이므로 RDWR로 오픈
	if(access("ssu_crontab_log",F_OK)<0)
		open("ssu_crontab_log",O_RDWR);

	stat("ssu_crontab_file",&statbuf);
	now=statbuf.st_mtime;//수정시간 받아옴
	while(1){

		while(fscanf(fp,"%[^\n]\n",command[cmdnum])>0)
			cmdnum++;


		for(int i=0;i<cmdnum;i++) {
			pthread_create(&tid[i],NULL,execute_thread,(void *)command[i]);
		}

		while(1){
			stat("ssu_crontab_file",&statbuf);
			if(now!=statbuf.st_mtime){
				now=statbuf.st_mtime;
				for(int i=0;i<cmdnum;i++)
					pthread_cancel(tid[i]);
				break;
			}
		}

	}
	gettimeofday(&end,NULL);
	print_runtime(&begin,&end);
	fclose(fp);
}

void print_runtime(struct timeval *begin, struct timeval *end) // 실행 시간 출력 
{
	end->tv_sec -= begin->tv_sec;
	if(end->tv_usec < begin->tv_usec){      
		end->tv_sec--;    
		end->tv_usec += SECOND_TO_MICRO;
	}
	end->tv_usec -= begin->tv_usec;   
	printf("Runtime: %ld:%ld(sec:usec)\n", end->tv_sec, end->tv_usec);  
}
void *execute_thread(void *cmd){

	char cmd_copy[MAX_TOKEN] = { 0 };
	char token[BUFFER_SIZE][MAX_TOKEN];
	char *tmp;
	char *last;
	int *timetable[5];
	char syscmd[BUFFER_SIZE];
	int argc=0;

	//시간측정
	time_t now;
	time_t exe;
	struct tm now_t;
	strcpy(cmd_copy,(char*)cmd);

	//타임테이블 초기화
	timetable[0] = calloc(60, sizeof(int));
	timetable[1] = calloc(24, sizeof(int));
	timetable[2] = calloc(32, sizeof(int));
	timetable[3] = calloc(13, sizeof(int));
	timetable[4] = calloc(8, sizeof(int));


	tmp=strtok_r(cmd_copy," ", &last);
	strcpy(token[argc++], tmp);

	//띄어쓰기를 기준으로 토큰을 분리
	while((tmp=strtok_r(NULL, " ", &last)) != NULL){
		strcpy(token[argc++], tmp);
	}

	memset(syscmd, 0, BUFFER_SIZE);
	for(int i=0;i<argc;i++){
		if(i<5)
			settable(token[i],timetable[i],i+1);
		else{
			strcat(syscmd,token[i]);
			strcat(syscmd," ");
		}
	}


	now=time(NULL);
	now_t= *localtime(&now);
	now_t.tm_sec=0;

	while(1){

		now_t.tm_min++;
		exe=mktime(&now_t);
		now_t= *localtime(&exe);
		now_t.tm_sec=0;

		if (timetable[0][now_t.tm_min] && 
				timetable[1][now_t.tm_hour] && 
				timetable[2][now_t.tm_mday] && 
				timetable[3][now_t.tm_mon + 1] && 
				timetable[4][now_t.tm_wday]) { // 실행 시간이 되었을 경우 
			now=time(NULL);//시간 측정

			//대기 시간 = 수행 예정 시간-현재시간
			printf("대기시간 : %ld\n",exe-now);
			sleep(exe-now);
			system(syscmd);
			printf("실행!!\n");
			write_log((char *)cmd,RUN);
		}
	}
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

void settable(char *period, int *table, int type){
	int start,end;
	char *t;
	char *tmp;
	char token[BUFFER_SIZE][BUFFER_SIZE];
	char op;
	char a[BUFFER_SIZE],b[BUFFER_SIZE];
	int tcnt=0;

	switch(type){
		case MIN:
			start=0;
			end=59;
			break;
		case HOUR:
			start=0;
			end=23;
			break;
		case DAY:
			start=1;
			end=32;
			break;
		case MON:
			start=1;
			end=12;
			break;
		case WEEKDAY:
			start=0;
			end=6;
			break;
	}

	//주기문자열을 형식에 맞게 token배열에 집어넣는 과정

	//쉼표기준 분리
	tmp=strtok_r(period,",",&t);
	strcpy(token[tcnt++],tmp);

	while((tmp=strtok_r(NULL,",",&t))!=NULL)
		strcpy(token[tcnt++],tmp);

	for(int i=0;i<tcnt;i++){
		memset(a,0,BUFFER_SIZE);
		memset(b,0,BUFFER_SIZE);
		op=0;

		if(strchr(token[i],'/') && strchr(token[i],'-'))
			sscanf(token[i],"%[^/]%c%s",a,&op,b);
		else
			sscanf(token[i],"%[^/-]%c%s",a,&op,b);

		if(op=='/'){
			int num=atoi(b);
			if(strchr(a,'-')!=NULL){
				char c[BUFFER_SIZE],d[BUFFER_SIZE];
				char op2=0;
				sscanf(a,"%[^-]%c%s",c,&op2,d);
				int j=atoi(c);
				for(j;j<atoi(d);j++){
					if(num%atoi(d)==0){
						table[j]=1;
						num++;
					}
				}
			}
			else{
				int j=start;
				for(j;j<end;j++)
					if(num % atoi(b) ==0){
						table[j]=1;


					}
			}
		}

		else if(op=='-'){
			int j=atoi(a);
			for(j;j<atoi(b);j++)
				table[j]=1;
		}

		else{
			if(strchr(a,'*')==NULL){
				table[atoi(a)]=1;
			}
			else{
				for(int j=start;j<=end;j++) {
					table[j]=1;
				}
			}
		}
	}
}

