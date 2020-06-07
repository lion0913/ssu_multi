#include "ssu_rsync.h"

#define DEBUG
int main(int argc, char *argv[]){	
	struct timeval start,end;
	struct stat statbuf;

	char tmppath[BUFFER_SIZE];//임시 경로 저장공간

	getcwd(pwd,BUFFER_SIZE);//실행위치 경로 받아옴

	gettimeofday(&start,NULL);
	if(argc < 3){
		fprintf(stderr,"usage : ssu_rsync [option] <src> <dst>\n");
		exit(1);
	}
	for(int i=1;i<argc;i++){
		if(argv[i][0] == '-'){
			if(argv[i][1]=='r')
				printf("roption!\n");
			else if(argv[i][1]=='t')
				printf("toption!\n");
			else if(argv[i][1]=='m')
				printf("moption!\n");
			else{
				printf("wrong input!\n");
				exit(1);
			}
			continue;
		}
	}

	// <src>경로 받아오고 상태 확인
	if(access(argv[argc-2],F_OK)<0){
		fprintf(stderr,"access error for %s\n",argv[argc-2]);
		exit(1);
	}
	realpath(argv[argc-2],srcpath);
	lstat(srcpath,&statbuf);
	if(S_ISDIR(statbuf.st_mode)){
		isdir=1;
	}else
		isdir=0;


	// <dst>경로 받아오고 상태 확인
	if(access(argv[argc-1],F_OK)<0){
		fprintf(stderr,"access error for %s\n",argv[argc-1]);
		exit(1);
	}
	realpath(argv[argc-1],dstpath);
	lstat(dstpath,&statbuf);
	if(!S_ISDIR(statbuf.st_mode)){//디렉토리가 아닌 경우
		fprintf(stderr,"<dst> must be a directory!\n");
		printf("dstpath: %s\n",dstpath);
		exit(1);
	}


	printf("dstpath : %s\n",dstpath);
	printf("srcpath : %s\n",srcpath);

	memset(tmppath,0,BUFFER_SIZE);
	strncpy(tmppath,dstpath,getfilename(dstpath)-dstpath);
	printf("tmppath : %s\n",tmppath);

	if(chdir(tmppath)<0){
		fprintf(stderr,"chdir error : %s\n",tmppath);
		exit(1);
	}
	/*동기화 실행 전 미리 백업데이터를 만들어 놓음.
	  동기화 실패시에 백업 데이터를 바탕으로 파일을 다시 복구해야하기 때문
	 */
	backupdata(argc,argv);

	char command[BUFFER_SIZE];
	sprintf(command,"tar -cf %s.swp %s",getfilename(dstpath),getfilename(dstpath));
	system(command);
	printf("압축파일 생성 성공!\n");

	chdir(pwd);
	signal(SIGINT,recover_file);
	execute_sync(srcpath,dstpath);//동기화 실행
	printf("sync실행\n");
	//생성된 스왑파일 삭제
	sprintf(tmppath,"%.*s.swp",(int)strlen(dstpath),dstpath);
	printf("스왑파일 삭제\n");
	unlink(tmppath);

	gettimeofday(&end,NULL);
	//ssu_runtime(&betin,&end);
	exit(0);
}
void backupdata(int argc, char *argv[]){//backupdata를 생성하는 함수
	backupargc=argc;
	backupargv=(char **)malloc(sizeof(char *)*argc);
	for(int i=0;i<argc;i++){
		backupargv[i]=argv[i];
	}
}
char *getfilename(char *path) // 파일명 추출 
{ 
	char *tmp = path; 
	int length = strlen(path); 
	for (int i = 0; i < length; i++) 
		if(path[i] == '/') 
			tmp = path + i; 
	return tmp + 1; 
} 
void recover_file(int signo){//파일을 복구하는 함수
	/*sigint 시그널을 받았을 경우 
	  dst 디렉토리 이전의 디렉토리로 이동 -> 기존 dst 디렉토리 삭제 
	  ->  생성했던 .swp 파일의 압축 해제 -> .swp파일 삭제
	 */
	char tmp[BUFFER_SIZE];
	char cmd[BUFFER_SIZE];
	if(signo==SIGINT)
	{	
		printf("sigint 캐치!\n");
		strncpy(tmp,dstpath,getfilename(dstpath)-dstpath);
		//printf("tmppath : %s\n",tmppath);
		chdir(tmp);

		remove_dir(dstpath);
		sprintf(cmd,"tar -xf %s.swp",getfilename(dstpath));
		system(cmd);
		unlink(cmd+9);
	}

}
void remove_dir(char *path){
	struct dirent *dirp; 
	struct stat statbuf; 
	DIR *dp; 
	char tmp[BUFFER_SIZE]; 

	if ((dp = opendir(path)) == NULL) 
		return; 
	while ((dirp = readdir(dp)) != NULL) {
		if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, "..")) 
			continue; 

		sprintf(tmp, "%s/%s", path, dirp->d_name);
		if (lstat(tmp, &statbuf) == -1) 
			continue; 
		if (S_ISDIR(statbuf.st_mode)) 
			remove_dir(tmp); 
		else  
			unlink(tmp); 
	} 
	closedir(dp); 
	rmdir(path); 
}
void execute_sync(char *srcpath,char*dstpath){
	f_tree *srctree;
	f_tree *dsttree;

	//파일 트리 생성하기(src, dst)
	srctree=make_tree(srcpath);
	dsttree=make_tree(dstpath);
	printf("srctree:%s\n",srctree);
	printf("srctree 새끼 :%s\n",srctree->child);
	isfirst=0;
	//src의 성격 확인하기(디렉토리/파일)
	if(S_ISDIR(srctree->statbuf.st_mode)){
		//디렉토리일 경우 안에 들어간 파일을 비교
		compare_tree(srctree->child,dsttree->child);
		write_change(srctree->child,CREATE);//비교했을때 새롭게 생성된 파일을 기록
	}
	else{
		//일반 파일인 경우 그 파일 자체와 dst디렉토리를 비교
		compare_file(srctree,dsttree->child);
		write_change(srctree,CREATE);
	}
	apply_change();//변경사항을 적용하는 함수

}

f_tree* make_tree(char *path){//파일 트리만들기
	int cnt; 
	int isfirst=1; 
	char tmp[BUFFER_SIZE]; 

	f_tree *head; 
	f_tree *now; 
	chdir(path);
	//memset(tmp,0,sizeof(char)*BUFFER_SIZE);
	head=make_node(); //트리 상태 초기화
	now=head; 
	strcpy(head->fname,path); 
	stat(head->fname,&(head->statbuf)); 
	cnt=scandir(".",&(head->namelist),NULL,alphasort);//디렉토리 안에 있는 모든 파일 읽어옴 
	//cnt=탐색할 파일의 개수 
	for(int i=0;i<cnt;i++){ 
		f_tree *new=make_node(); 
		if((!strcmp(head->namelist[i]->d_name,".")) ||(!strcmp(head->namelist[i]->d_name,".."))) 
			continue; 
		strcpy(new->fname,head->namelist[i]->d_name);//새로운 대의 파일이름을 생성 
		sprintf(tmp,"%s/%s",path,new->fname); 
		strcpy(new->fname,tmp); 
		stat(tmp,&(new->statbuf)); 
		if(S_ISDIR(new->statbuf.st_mode))//파일의 성격이 디렉토리라면 다시 트리를 만듦(재귀) 
			new=make_tree(tmp); 
		else 
			new->size=new->statbuf.st_size; 
		if(isfirst==1){ 
			now->child=new; 
			now=now->child; 
			isfirst=0; 
		} 
		else{ 
			now->sibling=new; 
			now=now->sibling; 
		} 
	}
	head->size=makesize(head);
	return head; 
} 
int makesize(f_tree *head)
{  
	int size; 
	size = 0; 
	if(S_ISDIR(head->statbuf.st_mode)) 
		head = head->child; 
	else  
		return head->statbuf.st_size; 
	while(head != NULL) { 
		size += head->size; 
		head = head->sibling; 
	} 
	return size; 
} 


f_tree* make_node(void){ //트리 상태를 초기화해주는 함수
	f_tree *tmp=calloc(1,sizeof(f_tree)); 
	memset(tmp->fname,0,BUFFER_SIZE); 
	tmp->sibling=NULL; 
	tmp->child=NULL; 
	tmp->namelist=NULL; 
	tmp->state=N; 
	tmp->size=0; 
	return tmp; 
} 
void compare_tree(f_tree *src,f_tree *dst){ 
	while(src!=NULL){ 
		compare_file(src,dst);//각 노드를 비교 
		if(src->child !=NULL)//자식을 가지고 있다면 자식을 먼저 비교 
			compare_tree(dst,src->child); 
		src=src->sibling;
	} 
} 


int compare_file(f_tree *src,f_tree *dst){ 
	while(dst!=NULL){
		//printf("compare_file들엉ㅁ\n");
		//[1]두 파일의 이름이 같은 경우
		if(!strcmp(src->fname+strlen(srcpath)-strlen(getfilename(srcpath)),dst->fname+strlen(dstpath)+1)){
			src->state=Y;
			//파일의 수정시간,크기, 형식이 다른 경우 -> 상태를 MODIFY로 변경
			if(src->statbuf.st_mtime != dst->statbuf.st_mtime){
				printf("compere_file : mtime is different!\n");
				src->state=MODIFY;
			}
			else if(src->size != dst->size){
				printf("compare_file : size is different!\n");
				src->state=MODIFY;
			}
			else if(src->statbuf.st_mode !=dst->statbuf.st_mode){
				printf("compare_file : type is different!\n");
				src->state=MODIFY;
			}
			dst->state=Y;
			return 1;
		}
		if(dst->child !=NULL)
			if(compare_file(src,dst->child))
				break;
		dst=dst->sibling;
	}
	return 0;
}
void write_change(f_tree *path,int state){
	changecnt=0;//카운트변수초기화
	char tmp[BUFFER_SIZE];
	//printf("write_change[path] : %s\n",path);

	while(path!=NULL){
		switch(path->state){
			case N : 
				if(state==CREATE){
					strcpy(changelist[changecnt].fname,path->fname);

					printf("changelist[%d] : %s\n",changecnt,changelist[changecnt].fname);
					changelist[changecnt].state=CREATE;
				}
				else if(state==DELETE){
					sprintf(tmp,"%.*s/%s",strlen(dstpath),dstpath,getfilename(dstpath));
					if(strstr(path->fname,tmp) ==NULL||!strcmp(path->fname,tmp))
						break;
					strcpy(changelist[changecnt].fname,path->fname);
					changelist[changecnt].state=DELETE;
					printf("changelist[%d] : %s\n",changecnt,changelist[changecnt].fname);
				}
				changelist[changecnt].size=path->size;
				changecnt++;
				break;

			case MODIFY:
				//수정상태일 경우 이름, 크기 changelist에 집어넣음
				strcpy(changelist[changecnt].fname, path->fname); 
				changelist[changecnt].state = MODIFY; 
				changelist[changecnt].size = path->size; 
				printf("changelist[%d] : %s\n",changecnt,changelist[changecnt].fname);
				changecnt++;
				break; 
		} 
		if(isfirst==1)
			if(path->sibling !=NULL){
				isfirst=0;
				write_change(path->child,state);
			}
		path=path->sibling;
	}
}

void apply_change(void){
	char path[BUFFER_SIZE];
	char buf[BUFFER_SIZE];
	struct stat statbuf;
	struct utimbuf namelist;
	int fd1,fd2;
	size_t length;

	sprintf(path,"%.*s",(int)strlen(dstpath),dstpath);
	printf("applychange[path]이동할 위치 : %s\n",path);
/*
	if(isdir==1 && access(path,F_OK)<0){
		//디렉토리이고 목적 파일에 해당 디렉토리가 없다면 디렉토리 생성
		//lstat(srcpath,&statbuf);
		printf("생성할 디렉토리 이름 : %s\n",path);
		//mkdir(path,statbuf.st_mode);
	}*/
	for(int i=0;i<changecnt;i++){
		switch(changelist[i].state){
			case CREATE :
			case MODIFY : 
				//printf("들어옴\n");
				memset(path, 0, BUFFER_SIZE); 
				printf("changelist: %s\n", changelist[i].fname);
				if(lstat(changelist[i].fname, &statbuf) < 0)
					printf("lstat error\n");
				sprintf(path, "%.*s/%s",strlen(dstpath),dstpath,changelist[i].fname + strlen(srcpath) + 1); 
				printf("renewal: path = %s\n", path); 

				if (S_ISDIR(statbuf.st_mode)){ 
					mkdir(path, 0755); 
					printf("디렉토리 생성 완료 : %s\n",path);
				}
				else { 
					if ((fd1 = open(changelist[i].fname, O_RDONLY)) < 0) {  
						fprintf(stderr, "open error for %s\n", changelist[i].fname); 
						break; 
					} 

					if ((fd2 = open(path, O_WRONLY | O_CREAT | O_TRUNC, statbuf.st_mode)) < 0) { // 동기화 파일 열기 
						fprintf(stderr, "open error for %s\n", path); 
						break; 
					} 
					while ((length = read(fd1, buf, BUFFER_SIZE)) > 0) // 동기화 파일 쓰기 (새로쓰기 혹은 덮어쓰기) 
						write(fd2, buf, length); 


					close(fd1); 
					close(fd2); 
				} 


				// 동기화 파일 속성 및 권한 복원 
				namelist.actime = statbuf.st_atime;  
				namelist.modtime = statbuf.st_mtime; 
				utime(path, &namelist);
				chmod(path, statbuf.st_mode); 
				break; 
		}
	}
	write_log();
}
void write_log(void){
	FILE *fp;
	time_t now;
	char tmp[BUFFER_SIZE];
	
	memset(tmp,0,BUFFER_SIZE);
	//로그 파일 오픈(없으면 생성)
	printf("pwd : %s\n",pwd);
	chdir(pwd);
	if((fp=fopen("ssu_rsync_log","r+"))==NULL){
		fp=fopen("ssu_rsync_log","w");
	}
	fseek(fp,0,SEEK_END);
	strcpy(tmp,"ssu_rsync");
	for(int i=1;i<backupargc;i++){
		strcat(tmp," ");
		strcat(tmp,backupargv[i]);
		//printf("backupargv[%d] : %s\n",i,backupargv[i]);
	}
	printf("tmp : %s\n",tmp);
	time(&now);


	fprintf(fp,"[%.24s] %s\n",ctime(&now),tmp);
	printf("changecnt:%d\n",changecnt);
	for(int i=0;i<changecnt;i++){
		switch(changelist[i].state){
			case CREATE:
			case MODIFY:
				if(isdir==1){

					fprintf(fp,"		%s %dbytes\n",changelist[i].fname+strlen(srcpath)+1,changelist[i].size);
					printf("		%s %dbytes\n",changelist[i].fname+strlen(srcpath)+1,changelist[i].size);
				}
				else{

					fprintf(fp,"		%s %dbytes\n",changelist[i].fname+strlen(srcpath)-strlen(getfilename(srcpath)),changelist[i].size);
					printf("		%s %dbytes\n",changelist[i].fname+strlen(srcpath)-strlen(getfilename(srcpath)),changelist[i].size);
				}
		}
	}
	fclose(fp);
}
