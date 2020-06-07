#include <stdio.h>
#include<pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>    
#include <malloc.h>  
#include <time.h>     
#include <errno.h>     
#include <signal.h>   
#include <dirent.h>   
#include <sys/wait.h>   
#define FILE_NAME 1024
#define BUFFER_SIZE 1024
#define MAX_TOKEN 15

#define MIN 1//0-59
#define HOUR 2//0-23
#define DAY 3//1-31
#define MON 4//1-12
#define WEEKDAY 5//0-6

#define ADD 1
#define REMOVE 2
#define RUN 3

char command[BUFFER_SIZE][BUFFER_SIZE];
int cmdnum=0;      
char *token[MAX_TOKEN];    

void prompt(void);//프롬프트 출력함수[저장된 명령어 출력 및 등록,삭제의 전반적인 구조 관리]
int format_check();
void write_file();
void write_log(char *input,int stat);
