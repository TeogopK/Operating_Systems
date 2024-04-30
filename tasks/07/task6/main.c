// Да се напише програма на С, която получава като параметър команда (без параметри) и при успешното ѝ изпълнение, извежда на стандартния изход името на командата.

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
		err(5, "Failed to write command name to stdout");
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
		if(wait(&status) < 0)
		{
			err(3, "Failed to wait the child process");
		}
		if(!WIFEXITED(status))
		{
			err(4, "Child was killed");
		}
		writeStr(command);
		writeStr("\n");

	}
}

int main (int args, char** argv)
{
	if(args != 2)
	{
		errx(2, "Invalid number of arguments");
	}

	forkLogic(argv[1]);

	exit(0);
}
