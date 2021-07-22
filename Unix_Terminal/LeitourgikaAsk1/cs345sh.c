/*EMMANOUIL SMYRNAKIS CSD3504*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>	
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

int deamon_process=0;
int fork_flag=0;
int file_Desc=0;

struct dirent_list{
	char* Dname;
	struct dirent_list *next;
};

struct dirent_list * header;

void type_prompt(char * buff,size_t bufSize){

	static int first_time=1;
	char * username; 
	char * path;

	if(first_time){
		const char * CLEAR_SCREEN_ANSI="\e[1;1H\e[2J";
		write(STDOUT_FILENO,CLEAR_SCREEN_ANSI,11);
		first_time=0;	
	}
	
	username=getlogin();
	if(getcwd(buff,bufSize)!=NULL){
		printf("%s@cs345sh/%s/$ ",username,buff);
	}else{
		perror("getcwd() error\n");
	}
}

void redirection(char** buffer, char** rest, int state){

        char* filename;
        FILE* f;

        filename=rest[1];

        if(state==1){
                f=fopen(filename,"w+");
		file_Desc = dup(fileno(stdout));	
                dup2(fileno(f),fileno(stdout));
                fclose(f);
        }

        if(state==2){
                f=fopen(filename,"a");
		file_Desc = dup(fileno(stdout));	
                dup2(fileno(f),fileno(stdout));
                fclose(f);
        }

        if(state==3){
                f=fopen(filename,"r");
		file_Desc = dup(fileno(stdin));	
                dup2(fileno(f),fileno(stdin));
                fclose(f);
        }
}

void changeDir(char **buffer){

        char *temp_buff=(char*)malloc(PATH_MAX);
	char * temp;
        if(buffer[1]==NULL){
                chdir(getenv("HOME"));
		getcwd(temp_buff,PATH_MAX);
        }else{
		chdir(buffer[1]);
		getcwd(buffer[1],PATH_MAX);
        }
}

void pipe_function(char ** buffer, char ** rest){
	
	int pipefd[2];
	pid_t pid;

	char buf;

	if(pipe(pipefd)==-1){
		perror("pipe");
		exit(1);
	}
	pid=fork();
	
	if(pid==-1){
		perror("fork");
		exit(1);
	}else if(pid==0){
		dup2(pipefd[1],STDOUT_FILENO);
		close(pipefd[1]);
		close(pipefd[0]);
		execvp(buffer[0],buffer);
	}else{
		pid=fork();
		if(pid==0){
			dup2(pipefd[0],STDIN_FILENO);
			close(pipefd[0]);
			close(pipefd[1]);
			execvp(rest[0],rest);
		}else{
			int status;
			close(pipefd[0]);
			close(pipefd[1]);
			waitpid(pid,&status,0);
		}
	}

}


void print_directory(char** buffer){
	
	struct dirent *de;
	struct dirent_list *tempD;
	struct dirent_list * iterator;
	struct stat info;
	char* temp;

	temp=(char*)malloc(PATH_MAX);
	
	getcwd(temp,PATH_MAX);
	DIR* dr=opendir(temp);

	if(buffer[1]==NULL){
		if(dr==NULL){
			perror("Cant open the directory\n");
			exit(1);
		}
		while((de=readdir(dr))!=NULL){
			printf("%s\n",de->d_name);
		}
		closedir(dr);
	}else if(strcmp(buffer[1],"-l")==0){
		if(dr==NULL){
			perror("Cant open the directory\n");
			exit(1);
		}
		while((de=readdir(dr))!=NULL){
			if(stat(de->d_name,&info)<0){
				perror("Stat");
				exit(1);
			}
			printf("%s\n",de->d_name);
			printf("---------------------------\n");
    		printf("File Size: \t\t%ld bytes\n",info.st_size);
    		printf("Number of Links: \t%ld\n",info.st_nlink);
    		printf("File inode: \t\t%ld\n",info.st_ino);

    		printf("File Permissions: \t");
    		printf( (S_ISDIR(info.st_mode)) ? "d" : "-");
    		printf( (info.st_mode & S_IRUSR) ? "r" : "-");
    		printf( (info.st_mode & S_IWUSR) ? "w" : "-");
    		printf( (info.st_mode & S_IXUSR) ? "x" : "-");
    		printf( (info.st_mode & S_IRGRP) ? "r" : "-");
    		printf( (info.st_mode & S_IWGRP) ? "w" : "-");
    		printf( (info.st_mode & S_IXGRP) ? "x" : "-");
   	 		printf( (info.st_mode & S_IROTH) ? "r" : "-");
    		printf( (info.st_mode & S_IWOTH) ? "w" : "-");
    		printf( (info.st_mode & S_IXOTH) ? "x" : "-");
    		printf("\n\n");

		}
		closedir(dr); 
	}else if(strcmp(buffer[1],"-r")==0){
		if(dr==NULL){
			perror("Cant open the directory\n");
			exit(1);
		}
		
		while((de=readdir(dr))!=NULL){
			if(stat(de->d_name,&info)<0){
				perror("Stat");
				exit(1);
			}
			
			tempD = (struct dirent_list *)malloc(sizeof(struct dirent_list));
			tempD->Dname = de->d_name;
			tempD->next = header;
			header = tempD;
		}
			iterator=header;

			while(iterator->next!=NULL){
				printf("%s\n",iterator->Dname);
				iterator=iterator->next;
			}

		closedir(dr); 
		header=NULL;
	}	
}

void print_contents(char ** buffer){
	
	FILE *fp;
	char* filename,ch;

	filename=buffer[1];
	fp=fopen(filename,"r");

	if(fp==NULL){
		printf("No such file or directory\n");
		return;
	}

	while((ch=fgetc(fp))!=EOF){
		putchar(ch);
	}
	fclose(fp);
}

char** read_command(){

	char **buffer,**rest;
	int i=0;
	int j=0;
	int status=0;	
	char *string;
	char *token;

	string =(char *)malloc(124 * sizeof(char));
	fgets(string, 124, stdin);

	token = (char *)malloc(124 * sizeof(char));

	buffer=(char**)malloc(124 * sizeof(char*));
        rest=(char**)malloc(124 * sizeof(char*));

	token = strtok(string, " \n=");

	while(token != NULL){
		if(strcmp(token, "<") == 0 || strcmp(token, ">") == 0 || strcmp(token, ">>") == 0){
			status = 1;
			break;
		}else if(strcmp(token, "|") == 0){
			status = 2;
			break;
		}else if(strcmp(token, "&") == 0){
			status = 3;
			break;
		}
		buffer[i] = token;
		token = strtok(NULL, " \n=");
		i++;	
	}

	if(status==1){
		while(token != NULL){
			rest[j] = token;
			token = strtok(NULL, " \n=");
			j++;
		}
	}else if(status==2){
		while(token != NULL){
			if(strcmp(token, "|") == 0){
				token = strtok(NULL, " \n=\"\'");
			}
		rest[j] = token;
		token = strtok(NULL, " \n=\"\'");
		j++;
		}
	}else if(status==3){
		rest[j]="&";
		deamon_process=1;
	}
	buffer[i]=NULL;
	rest[j]=NULL;

	if(rest[0] != NULL && status == 1){
		if(strcmp(rest[0],">")==0){
			redirection(buffer,rest,1);
		}else if(strcmp(rest[0],">>")==0){
			redirection(buffer,rest,2);
		}else if(strcmp(rest[0],"<")==0){
			redirection(buffer,rest,3);
		}
	}

	if(rest[0]!=NULL && status==2){
		fork_flag=1;
		pipe_function(buffer,rest);
	}

	if(strcmp(buffer[0],"exit")==0){
		fork_flag=1;
		exit(1);
	}else if(strcmp(buffer[0],"cd")==0){
		fork_flag=1;
		changeDir(buffer);
	}

	if(strcmp(buffer[0],"ls")==0){
		fork_flag=1;
		print_directory(buffer);
	}

	if(strcmp(buffer[0],"cat")==0){
		fork_flag=1;
		print_contents(buffer);
	}

return buffer;

}

int main(){

	char **parameters,buff[PATH_MAX];
	int i=0;
	pid_t pid;
	int status=0;
	
	while(1){
	
	
	 type_prompt(buff,PATH_MAX);

        parameters=read_command();

		if(fork_flag==0){
			pid=fork();
			if(pid>0){
				waitpid(-1,&status,0);
			}else if(pid==0){
				execvp(parameters[0],parameters);
			}
		}

		deamon_process = 0;
		fork_flag = 0;
		
	dup2(file_Desc, fileno(stdout)); //go to shell
	dup2(file_Desc, fileno(stdin));
	}
	return 0;
}
