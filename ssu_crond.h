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
#include <pthread.h>
#include<sys/time.h>

#define SECOND_TO_MICRO 100000

#define MIN 1//0-59
#define HOUR 2//0-23
#define DAY 3//1-31
#define MON 4//1-12
#define WEEKDAY 5//0-6

#define FILE_NAME 1024  
#define BUFFER_SIZE 1024   
#define MAX_TOKEN 15     

#define ADD 1  
#define REMOVE 2 
#define RUN 3

char command[BUFFER_SIZE][BUFFER_SIZE];
int cmdnum;

void print_runtime(struct timeval *begin,struct timeval *end);
void write_log(char *input,int stat);
void *execute_thread(void *cmd);
void settable(char *period,int *table,int type);
