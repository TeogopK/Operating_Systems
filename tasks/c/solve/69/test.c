#include <err.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

const int size = 20;

int main(int argc, char** argv) {

	int fd = open(argv[1], O_RDONLY);
	int out = open("oout", O_WRONLY | O_CREAT | O_TRUNC, 0666);

	uint8_t a;
	uint16_t b;

	int read_count = 0;

	//file is made of uint8_t, but we save in uint16_t
	

	lseek(fd, 1, SEEK_SET);
	while( (read_count = read(fd, &b, sizeof(a))) > 0) {

		dprintf(1, "%d ", b);
		if(write(out, &b, sizeof(a)) < 0) {
			err(3, "writing");
		}
	}

	if(read_count < 0) {
		err(1, "Error reading");
	}

	dprintf(1, "\n");

	exit(0);
}
