#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <err.h>
#include <stdlib.h>

void unlinkFifo(void);

void unlinkFifo(void) {
	if(unlink("temp") < 0) {
		err(10, "Error while unlinking Fifo");
	}
}

int main(int argc, char** argv) {

	if(argc != 2) {
		errx(1, "Usage is %s <file>", argv[0]);
	}

	if(mkfifo("temp", 0666) < 0) {
		err(2, "Error while creating Fifo");
	}

	int fd = open("temp", O_WRONLY);

	if(fd < 0) {
		unlinkFifo();
		err(3, "Failed to open Fifo");
	}

	pid_t pid = fork();

	if(pid < 0) {
		unlinkFifo();
		err(4, "Error while forking");
	}

	if(pid == 0) {
		if(dup2(fd, 1) < 0) {
			err(5, "Error while duplicating descrtiptor");
		}

		if(execlp("cat", "cat", argv[1], (char *)NULL) < 0) {
			err(6, "Failed to exec cat");
		}

	}

	int status;

	if(wait(&status) < 0) {
		unlinkFifo();
		err(8, "Error while waiting child");
	}

	unlinkFifo();

	close(fd);
	exit(0);
}
