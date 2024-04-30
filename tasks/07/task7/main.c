#include <string.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void writeStr(const char* str);
void executeComand(const char* str);
void forkLogic(const char* str);

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
	if(dprintf(1, num) < 0)
	{
		err(51, "Failed to print num to stdout");
	}
}

void printPid(const pid_t pid)
{
	if(dprintf(1, pid) < 0)
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
		printPid(command);
		writeStr("\n");
		
	}
}

int main (int args, char** argv)
{
	if(args != 2)
	{
		errx(2, "Invalid number of arguments");
	}

	for(int i = 1; i <= 3; i++)
	{
		forkLogic(argv[i]);
	}

	for(int i = 1; i <= 3; i++)
	{
		wait();
	}

	exit(0);
}
