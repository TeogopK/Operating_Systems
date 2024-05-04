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
		errx(1, "Usage %s <program>", argv[0]);
	}

	int fd = open("temp", O_RDONLY);

	if(fd < 0) {
		unlinkFifo();
		err(3, "Failed to open Fifo");
	}

	if(dup2(fd, 0) < 0) {
		err(4, "Error while duplicating descrtiptor");
	}

	if(execl(argv[1], argv[1], (char *)NULL) < 0) {
		err(5, "Failed to exec %s", argv[1]);
	}
	
}
