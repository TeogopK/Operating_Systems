// An example implementation of a shell console.
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <err.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

void showPrompt(void);
void getCmdAsString(char* buff);
void convertCmdToArray(char* cmd, char** cmdArgv);
void executeCommand(char* cmd);
void executeCommandInProcess(char** cmdArgv);

const char CMD_END = '\n';
const char STRING_TERMINATOR = '\0';
const uint16_t MAX_BUFF_SIZE = 4096;
const uint16_t MAX_NUM_OF_ARGS = 100;

void showPrompt(void)
{
	const char promptText[] = "> ";

	if (write(1, &promptText, strlen(promptText)) == -1)
	{
		err(1, "Error while showing prompt to stdout");
	}
}

void getCmdAsString(char* buff)
{
	int16_t count_bytes = 0;
	char sym = 'a'; //basic value

	uint16_t i = 0;
	while(i < MAX_BUFF_SIZE - 1)
	{	
		count_bytes = read(0, &sym, sizeof(sym));

		if(count_bytes == -1)
		{
			err(2, "Error while reading user input");
		}

		if(sym == CMD_END || sym == STRING_TERMINATOR)
		{
			break;
		}

		buff[i] = sym;
		i++;
	}

	buff[i] = STRING_TERMINATOR;
}

void convertCmdToArray(char* cmd, char** cmdArgv)
{
	uint16_t j = 0;
	uint16_t startOfNextArgument = 0;
	for(uint16_t i = 0; cmd[i] != STRING_TERMINATOR; i++)
	{
		if(cmd[i] == ' ')
		{
			cmd[i] = STRING_TERMINATOR;
			//cmdArgv[j] = &cmd[startOfNextArgument]; //an alternative
			cmdArgv[j] = cmd + startOfNextArgument;
			j++;

			startOfNextArgument = i + 1;
		}
	}

	cmdArgv[j] = cmd + startOfNextArgument; //last argument has no space after
	cmdArgv[j + 1] = NULL;
}

void executeCommand(char* cmd)
{
	char* cmdArgv[MAX_NUM_OF_ARGS];
	convertCmdToArray(cmd, cmdArgv);

	if(strcmp(cmdArgv[0], "exit") == 0)
	{
		exit(0);
	}


	pid_t childPid = fork();

	if(childPid < 0)
	{
		err(3, "Could not fork process to start command");
	}

	if(childPid == 0)
	{
		executeCommandInProcess(cmdArgv);
	}

	int status;
	wait(&status);

	if(!WIFEXITED(status))
	{
		warnx("Command was killed");
		return;
	}

}

void executeCommandInProcess(char** cmdArgv)
{
	int execStatus = execvp(cmdArgv[0], cmdArgv);
	if(execStatus < 0)
	{
		warn("Could not execute command %s", cmdArgv[0]);
	}
}

int main (int argc, char** argv)
{
	char cmd [MAX_BUFF_SIZE];

	while(true)
	{
		showPrompt();
		getCmdAsString(cmd);

		executeCommand(cmd);
	}
}
