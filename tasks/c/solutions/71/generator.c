#include <stdint.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char** argv) {

	int fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
	uint8_t arr [3];

	for(int i = 0 ; i < 3; i++) {
		arr[i] = 11 + i;
	}

	write(fd, arr, 3 * 1);

	close(fd);
	exit(0);
}
