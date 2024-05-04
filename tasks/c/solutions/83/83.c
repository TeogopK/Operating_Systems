#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>

const int MIN_ARG = 3;
const int MAX_ARG = 100;

bool timeCheck(uint64_t start, uint64_t end, uint64_t seconds);

bool timeCheck(uint64_t start, uint64_t end, uint64_t seconds) {
	return end - start < seconds;
}

int main(int argc, char** argv)
{
	if(argc < MIN_ARG) {
		errx(1, "Invalid number of parameters");
	}

	int seconds = atoi(argv[1]);
	if(seconds < 1 || seconds > 9) {
		errx(2, "Invalid seconds %d as parameter", seconds);
	}

	if(argc > MAX_ARG) {
		errx(4, "Too many arguments");
	}

	int exitCode;
	int failCount = 0;

	char* arr [MAX_ARG];
	int i = 0;
	
	for(int j = MIN_ARG - 1; j < argc; j++)
	{
		arr[i] = argv[j];
		i++;
	}

	arr[i] = NULL;

	int fd = open("run.log", O_CREAT | O_TRUNC | O_WRONLY, 0666);
	if(fd < 0) {
		err(10, "Error while opening log file");
	}

	while(true) {
		pid_t pid = fork();

		if(pid < 0) {
			err(3, "Error while forking process");
		}

		if(pid == 0) {
			if(execvp(arr[0], arr) < 0) {
				err(5, "Error failed to exec %s", arr[0]);
			}
		} else {
			uint64_t start = time(NULL);
			int status;

			if(wait(&status) < 0) {
				err(6, "Error failed to wait child");
			}

			uint64_t end = time(NULL);
			

			dprintf(1, "\nStart time: %ld	", start);
			dprintf(1, "End time: %ld\n", end);
			dprintf(1, "Wait: %ld	 Allowed: %d\n", end - start, seconds);

			if(!WIFEXITED(status)) {
				exitCode = 129;
			}
			else {
				exitCode = WEXITSTATUS(status);
			}	
			dprintf(1, "Exit code: %d\n", exitCode);
			
			//the legit dprintf
			dprintf(fd, "%ld %ld %d\n", start, end, exitCode);

			if(exitCode != 0 && timeCheck(start, end, seconds)) {

				dprintf(1, "Failed!\n");
				failCount++;
			} else {
				failCount = 0;
			}

			if(failCount > 1) {
				dprintf(1, "Failed twice in a row!! Ending...\n");
				break;
			}
				
		}
	}

	close(fd);
	exit(0);
}
