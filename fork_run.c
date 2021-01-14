/**Discussed with George wgt7xp*/
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
int my_system(const char *command)
{
	pid_t pid = fork();
	if(pid==-1)
		return -1;
	else if(pid==0)
	{
		 execl("/bin/sh", "sh", "-c", command, (char *) NULL);
	}
	else
	{
		int status;
		waitpid(pid, &status,0);
		return status;
	}
	return -1;
}
char *getoutput(const char *command)
{
	int fd[2];
	if(pipe(fd)==-1)
		return -1;
	pid_t pid = fork();
	if(pid==-1)
		return -1;
	else if(pid==0)
	{
		 dup2(fd[1],1);
		 close(fd[0]);
		 close(fd[1]);
		 execl("/bin/sh", "sh", "-c", command, (char *) NULL);
	}
	else
	{
		int status;
		close(fd[1]);
		size_t size = 4096;
		char* read = malloc(size);
		FILE* pipe_read = fdopen(fd[0],"r");
		getdelim(&read,&size,'\0', pipe_read);

		waitpid(pid, &status,0);
		return read;
	}
	return -1;
	
}

