#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <err.h>
#include <unistd.h>

// Thi is a C-program that executes:
// cut -f7 -d: /etc/passwd | sort | uniq -c | sort -nr

const int MAX_ARGUMENTS = 100;
const int MAX_COMMAND = 1024;

void waitChild(void);
void dupDescriptor(int left, int right);
void executeCommandFromString(const char* command);
void getVectorFromCommand(char* command, char** commandArr);
void executeCommand(char** commandArr);
int doFork(void);

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
		err(2, "Error: failed to execute commandArr %s", commandArr[0]);
	}
}


void executeCommandFromString(const char* str)
{
	// how important are these
	if(str == NULL){
		errx(10, "Error: command is Null");
	}

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
		err(3, "Error: failed to wait the child process");
	}

	//is this necessary
	if(!WIFEXITED(status)){
		errx(20, "Child did not exited normally");
	}
}

void dupDescriptor(int left, int right) {
	if(dup2(left, right) < 0) {
		err(3,"Error: failed to duplicate pipe descriptor");
	}
}

int doFork(void)
{
	int pid = fork();

	if(pid < 0){
		err(1, "Error: fork failed");
	}

	return pid;
}

int main(int argc, char** argv)
{
	if(argc != 1) {
		//how to avoid unused variable warnings;
		errx(100, "Error: no parameters needed for %s", argv[0]);
	}

	const char* commands [] = { "cut -f7 -d: /etc/passwd",  "sort",  "uniq -c",  "sort -nr" };

	int pipefd[2];
	if(pipe(pipefd) < 0) {
		err(5, "Error: pipe was not created successfully");
	}

	int pid = doFork();

	if(pid == 0){
		close(pipefd[0]);
		dupDescriptor(pipefd[1], 1);

		executeCommandFromString(commands[0]);
	} else{
		close(pipefd[1]);
		waitChild();
	}

	/////////////////////////////////
	
	int nextPipefd[2];
	if(pipe(nextPipefd) < 0) {
		err(5, "Error: pipe was not created successfully");
	}

	pid = doFork();

	if(pid == 0){
		close(nextPipefd[0]);
  
		dupDescriptor(nextPipefd[1], 1);
		dupDescriptor(pipefd[0], 0);

		executeCommandFromString(commands[1]);
	} else{
		close(nextPipefd[1]);
		waitChild();
	}

	//##########################################
	
	int third[2];
	if(pipe(third) < 0) {
		err(5, "Error: pipe was not created successfully");
	}

	pid = fork();

	if(pid < 0){
		err(1, "Error: fork failed");
	}

	if(pid == 0){
		close(third[0]);
  
		dupDescriptor(third[1], 1);
		dupDescriptor(nextPipefd[0], 0);

		executeCommandFromString(commands[2]);
	} else{
		close(third[1]);
		waitChild();
	}

	dup2(third[0], 0);
	executeCommandFromString(commands[3]);
}
