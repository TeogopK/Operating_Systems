#include <err.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>

// cat argv[1] | sort | uniq

void executeInChild(int closeChild, int closeParent, int stdinChild, int stdoutChild, int stdinParent, int stdoutParent, char** commandArg);

void executeInChild(int closeChild, int closeParent, int stdinChild, int stdoutChild, int stdinParent, int stdoutParent, char** commandArg)
{	
	pid_t pid = fork();

	if(pid < 0) {
		err(3, "Error: Failed to fork");
	}

	if(pid == 0) {
		close(closeChild);

		if(dup2(stdinChild, 0) < 0){
			err(5, "Error while duplicating stdin of child");
		}

		if(dup2(stdoutChild, 1) < 0){
			err(5, "Error while duplicating stdout of child");
		}


		if(execvp(commandArg[0], commandArg) < 0) {
			err(6, "Error failed to execute %s",  commandArg[0]);
		}

	} else {
		close(closeParent);

		int status;
		if(wait(&status) < 0) {
			err(7, "Failed to wait the child process to end");
		}

		if(!WIFEXITED(status)) {
			err(8, "The child did not exited normally");
		}

		if(WEXITSTATUS(status) != 0) {
			err(9, "The child did not exited successfully");
		}

		if(dup2(stdinParent, 0) < 0){
			err(5, "Error while duplicating stdin of parent");
		}

		if(dup2(stdoutParent, 1) < 0){
			err(5, "Error while duplicating stdout of parent");
		}
	}

}

int main(int argc, char ** argv)
{
	if(argc != 2)
	{
		errx(1, "Invalid number of parameters");
	}


	int a[2];
	if(pipe(a) < 0) {
		err(2, "Error failed to create a pipe");
	}

	char cat [10];
	strcpy(cat, "cat");
	char* commandCat [] = {cat, argv[1], 0};
	executeInChild(a[0], a[1], 0, a[1], 0, 1, commandCat); 

	int b[2];
	if(pipe(b) < 0) {
		err(2, "Error failed to create a pipe");
	}

	char sort [10];
	strcpy(sort, "sort");
	char* commandSort [] = {sort, 0};
	executeInChild(b[0], b[1], a[0], b[1], b[0], 1, commandSort); 

	if(execlp("uniq", "uniq", 0) < 0) {
		err(6, "Error failed to execute sort");
	}


}

