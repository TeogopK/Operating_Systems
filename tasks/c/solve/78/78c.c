#include <sys/wait.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>

// cat argv[1] | sort | uniq

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

	pid_t pid = fork();

	if(pid < 0) {
		err(3, "Error: Failed to fork");
	}

	if(pid == 0) {
		close(a[0]);

		if(dup2(a[1], 1) < 0){
			err(5, "Error while duplicating descriptor");
		}

		if(execlp("cat", "cat", argv[1], 0) < 0) {
			err(6, "Error failed to execute cat");
		}

	} else {
		close(a[1]);

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

	}

	/////////////////////////////////////////////////

	int b[2];
	if(pipe(b) < 0) {
		err(2, "Error failed to create a pipe");
	}

	pid = fork();

	if(pid < 0) {
		err(3, "Error: Failed to fork");
	}

	if(pid == 0) {
		close(b[0]);

		if(dup2(a[0], 0) < 0){
			err(5, "Error while duplicating descriptor");
		}

		if(dup2(b[1], 1) < 0){
			err(5, "Error while duplicating descriptor");
		}

		if(execlp("sort", "sort", 0) < 0) {
			err(6, "Error failed to execute sort");
		}

	} else {
		close(b[1]);

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

		if(dup2(b[0], 0) < 0){
			err(5, "Error while duplicating descriptor");
		}
	}

	//#################################

	if(execlp("uniq", "uniq", 0) < 0) {
		err(6, "Error failed to execute uniq");
	}


}
