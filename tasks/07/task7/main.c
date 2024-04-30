// Да се напише програма на С, която получава като параметри три команди (без параметри), изпълнява ги последователно, като изчаква края на всяка и извежда на стандартния изход номера на завършилия процес, както и неговия код на завършване.

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void writeStr(const char* str);
void executeComand(const char* str);
void forkLogic(const char* str);
void printInt(const int num);
void printPid(const pid_t pid);

void executeComand(const char* command)
{
	if(execlp(command, command, (char *) NULL) < 0)
	{
		err(5, "Failed to exec the command");
	}
}

void writeStr(const char* str)
{
	if(write(1, str, strlen(str)) < 0)
	{
		err(50, "Failed to write command name to stdout");
	}
}

void printInt(const int num)
{
	if(dprintf(1, "%d", num) < 0)
	{
		err(51, "Failed to print num to stdout");
	}
}

void printPid(const pid_t pid)
{
	if(dprintf(1, "%d", pid) < 0)
	{
		err(52, "Failed to print pid to stdout");
	}
}

void forkLogic(const char* command) 
{
	pid_t pid = fork();

	if(pid < 0)
	{
		err(1, "Failed to fork a new process");
	}

	int status;
	if(pid == 0)
	{
		executeComand(command);
	}
	else
	{
		int childPid = wait(&status);
		if(childPid < 0)
		{
			err(3, "Failed to wait the child process");
		}
		if(!WIFEXITED(status))
		{
			err(4, "Child was killed");
		}
		printPid(childPid);
		writeStr(" with status ");
		printInt(status);
		writeStr("\n");
	}
}

int main (int args, char** argv)
{
	if(args != 4)
	{
		errx(2, "Invalid number of arguments");
	}

	for(int i = 1; i <= 3; i++)
	{
		forkLogic(argv[i]);
	}

	for(int i = 1; i <= 3; i++)
	{
		wait(NULL);
	}

	exit(0);
}
