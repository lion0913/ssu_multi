#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <fcntl.h> 
#include <dirent.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/time.h>
#include<signal.h>
#include<utime.h>
#include<string.h>
#include<time.h>
#include<sys/stat.h>

#define BUFFER_SIZE 1024

#define N -1
#define Y 0
#define CREATE 1
#define MODIFY 2
#define DELETE 3


char pwd[BUFFER_SIZE];//실행위치의 경로 받아오는 함수
char srcpath[BUFFER_SIZE];//src의 경로
char dstpath[BUFFER_SIZE];//dst의 경로

int isdir=0;//src가 디렉토리인지, 일반 파일인지 알게 하는 전역변수
int isfirst=1;//처음인지 아닌지 판별하는 스위치의 역할을 하는 전역변수
int changecnt=0;//changelist의 카운트를 세는 변수


//백업을 위한 전역변수(argc, argv인자를 저장)
int backupargc;
char **backupargv;

//파일 트리 구조체[srcpath에서의 트리1/ dstpath에서의 트리2]
typedef struct f_tree{ 
	char fname[BUFFER_SIZE];//파일이름 
	struct dirent **namelist; 
	struct stat statbuf; 
	struct f_tree *sibling; 
	struct f_tree *child; 
	int state; 
	int size; 
}f_tree; 

f_tree changelist[BUFFER_SIZE];

char *getfilename(char *path);
//절대경로에서 파일 이름만을 추출하는 함수.[인자 : 절대경로]
void execute_sync(char *srcpath,char*dstpath);
//동기화를 실행하는 함수[인자: <src>경로, <dst>경로]
void recover_file(int signo);
void backupdata(int argc, char *argv[]);    
//백업데이터 생성하는 함수, 동기화 실패 시 백업 데이터를 바탕으로 다시 복구하기 위해 필요
//[인자 : argc, argv[](입력값)], 리턴값은 없지만 전역변수인 backupargc, backupargv를 차례로 채움.
void remove_dir(char *path);
void write_change(f_tree *path, int state);
f_tree* make_tree(char *path);
f_tree* make_node(void);
void compare_tree(f_tree *src,f_tree *dst);
int compare_file(f_tree *src,f_tree *dst);
//src,dst 파일 비교를 하고 state를 채우는 함수
//[정상적으로 끝낸 경우 1을 리턴, 그게 아니라면 0을 리턴]
int makesize(f_tree *head);
//디렉토리 & 파일의 사이즈를 구조체에 저장하는 함수
void apply_change(void);
//변경사항을 적용하는 함수
void write_log(void);
