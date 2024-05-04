#include <err.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>

// cat argv[1] | sort

int main(int argc, char ** argv)
{
	if(argc != 2)
	{
		errx(1, "Invalid number of parameters");
	}

	int pipefd[2];
	if(pipe(pipefd) < 0) {
		err(2, "Error failed to create a pipe");
	}

	pid_t pid = fork();

	if(pid < 0) {
		err(3, "Error: Failed to fork");
	}

	if(pid == 0) {
		if(close(pipefd[0]) < 0){
			err(4, "Error while closing pipe end");
		}
		if(dup2(pipefd[1], 1) < 0){
			err(5, "Error while duplicating descriptor");
		}

		if(execlp("cat", "cat", argv[1], 0) < 0) {
			err(6, "Error failed to execute cat");
		}

	} else {
		if(close(pipefd[1]) < 0){
			err(4, "Error while closing pipe end");
		}

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

		if(dup2(pipefd[0], 0) < 0){
			err(5, "Error while duplicating descriptor");
		}
	}

	if(execlp("sort", "sort", 0) < 0) {
		err(6, "Error failed to execute sort");
	}

	exit(0);
}

