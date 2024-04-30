#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

const char* DEFAULT_COMMAND = "echo";
const int MAX_ARGS = 4;
const int MAX_SMALL_ARGS = 2;
const int MAX_STRING_SIZE = 4;

int main(int argc, char** argv)
{
	char command [MAX_STRING_SIZE + 1];

	if(argc == 1) {
		strcpy(command, DEFAULT_COMMAND);
	}

	else if(argc == 2) {
		if(strlen(argv[1]) > MAX_STRING_SIZE) {
			errx(1, "Error command has more than the allowed %d symbols", MAX_STRING_SIZE);
		}

		strcpy(command, argv[1]);
	}

	else {
		errx(2, "Error invalid number of parameters");
	}

	int bytes_count = 0;
	char sym;

	char arr [MAX_ARGS + 1][MAX_SMALL_ARGS + 1][MAX_STRING_SIZE + 1];
	int sizes[MAX_ARGS + 1];

	int argCount = 0;
	int totalArgCount = 0;
	int times = 0;
	int i = 0;

	while( (bytes_count = read(0, &sym, sizeof(char))) > 0) {
		if(totalArgCount >= MAX_ARGS) {
			errx(6, "Total arguments are more than the allowed count of %d", MAX_ARGS);
		}

		if(argCount >= MAX_SMALL_ARGS) {
			errx(5, "Arguments per exec are more than the allowed count of %d", MAX_SMALL_ARGS);
		}

		if(sym == ' ') {
			arr[times][argCount][i] = '\0';

			totalArgCount++;
			argCount++;

			i = 0;
			continue;
		}

		if(sym == '\n') {
			arr[times][argCount][i] = '\0';

			sizes[times] = argCount + 1;
			totalArgCount++;
			times++;

			argCount = 0;
			i = 0;

			continue;
		}

		if(i >= MAX_STRING_SIZE) {
			errx(4, "Error command arguments has more than the allowed %d symbols", MAX_STRING_SIZE);
		}

		arr[times][argCount][i] = sym;
		i++;
	}

	if(bytes_count < 0) {
		err(3, "Error while reading input from stdin");
	}

	for(int j = 0; j < times; j++) {
		//dprintf(1, "In cycle %d\n", j);

		pid_t pid = fork();

		if(pid < 0) {
			err(7, "Error while forking process");
		}

		if(pid == 0) {
			char* arrToExec [MAX_SMALL_ARGS + 2];

			arrToExec[0] = command;
			for(int s = 1; s <= sizes[j]; s++)
			{
				arrToExec[s] = arr[j][s - 1];
			}
			arrToExec[sizes[j] + 1] = NULL;

			if(execvp(command, arrToExec) < 0) {
				err(9, "Failed to execute command %s in child", command);
			}

		}
		else {
			int status;

			if(wait(&status) < 0) {
				err(10, "Failed to wait for child");
			}

			if(!WIFEXITED(status)) {
				err(11, "The child did not exit normally");
			}

			if(WEXITSTATUS(status) != 0) {
				err(12, "Child did not finish successfully");
			}

		}
	}	


	exit(0);
}

