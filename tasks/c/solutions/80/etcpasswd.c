#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <err.h>
#include <unistd.h>

// This is a C-program that executes multiple piped commands like:
// cut -f7 -d: /etc/passwd | sort | uniq -c | sort -nr

const int MAX_ARGUMENTS = 100;
const int MAX_COMMAND = 1024;

void waitChild(void);
pid_t doFork(void);
void dupDescriptor(int left, int right);
void executeCommand(char** commandArr);

void executeCommandFromString(const char* command);
void getVectorFromCommand(char* command, char** commandArr);
void executeMultipleCommands(const char** commands);

void getVectorFromCommand(char* command, char** commandArr)
{
	int positionArr = 0;
	int lastPosition = 0;

	int i = 0;
	while(command[i] != '\0')
	{
		if(command[i] == ' ') {
			command[i] = '\0';
			commandArr[positionArr] = command + lastPosition;
			lastPosition = i + 1;
			positionArr++;
		}

		i++;
	}

	commandArr[positionArr] = command + lastPosition;
	commandArr[positionArr + 1] = NULL;

}

void executeCommand(char** commandArr)
{
	if(execvp(commandArr[0], commandArr) < 0) {
		err(5, "Error: failed to execute commandArr %s", commandArr[0]);
	}
}


void executeCommandFromString(const char* str)
{
	char command[MAX_COMMAND];
	strcpy(command, str);

	char* commandArr[MAX_ARGUMENTS];

	getVectorFromCommand(command, commandArr);	
	executeCommand(commandArr);
}

void waitChild(void)
{
	int status;
	if(wait(&status) < 0) {
		err(6, "Error: failed to wait the child process");
	}

	//is this necessary
	if(!WIFEXITED(status)){
		errx(7, "Child did not exited normally");
	}
}

void dupDescriptor(int left, int right) {
	if(dup2(left, right) < 0) {
		err(4,"Error: failed to duplicate pipe descriptor");
	}
}

pid_t doFork(void)
{
	pid_t  pid = fork();

	if(pid < 0){
		err(2, "Error: fork failed");
	}

	return pid;
}

void executeMultipleCommands(const char** commands)
{
	if(commands == NULL || commands[0] == NULL) {
		errx(101, "Error: no commands given to execute");
	}

	if(commands[1] == NULL) {
		executeCommandFromString(commands[0]);
	}

	///////////////////////////////////////
	// The first command is executed here

	int pfirst[2];
	if(pipe(pfirst) < 0) {
		err(3, "Error: pipe was not created successfully");
	}

	pid_t pid = doFork();

	if(pid == 0){
		close(pfirst[0]);
		dupDescriptor(pfirst[1], 1);

		executeCommandFromString(commands[0]);
	} else{
		close(pfirst[1]);
		waitChild();
	}

	/////////////////////////////////
	// All the commands in the middle are executed here

	int psecond[2];

	int i = 1;
	while(commands[i + 1] != NULL)
	{
		if(pipe(psecond) < 0) {
			err(3, "Error: pipe was not created successfully");
		}

		pid = doFork();

		if(pid == 0){
			close(psecond[0]);

			// the child reads from the read end of the first pipe,
			// where the previoud command has written
			// and writes to the end of the second pipe,
			dupDescriptor(psecond[1], 1);
			dupDescriptor(pfirst[0], 0);

			executeCommandFromString(commands[i]);
		} else{
			close(psecond[1]);
			waitChild();
		}

		close(pfirst[0]);

		// switches the only open pipe-end (READ of parrent, second pipe) to be accessed via the first pipe struct
		// so it is the input (stdin) of the next command
		pfirst[0] = psecond[0];

		i++;
	}

	/////////////////////////////////////
	// The last command is executed here

	dupDescriptor(pfirst[0], 0);
	close(pfirst[0]); //is this necessary

	executeCommandFromString(commands[i]);
}

int main(int argc, char** argv)
{
	if(argc != 1) {
		//how to avoid unused variable warnings;
		errx(100, "Error: no parameters needed for %s", argv[0]);
	}

	const char* commands [] = { "cut -f7 -d: /etc/passwd", "sort",  "uniq -c",  "sort -nr", "head -n 1", NULL};

	executeMultipleCommands(commands);
}
