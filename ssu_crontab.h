#include <stdio.h>
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

void prompt(void);//프롬프트 출력함수[저장된 명령어 출력 및 등록,삭제의 전반적인 구조 관리]
int format_check(char **token);
