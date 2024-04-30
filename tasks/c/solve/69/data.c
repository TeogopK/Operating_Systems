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

int openWrite(const char* file) {
	int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);

	if(fd < 0) {
		err(2, "Error while opening file %s", file);
	}

	return fd;
}

const int size = 20;

int main(int argc, char** argv) {

	int fd = openWrite(argv[1]);
	
	uint16_t arr [size];

	for(uint16_t i = 0; i < size; i++) {
		arr[i] = i;
	}

	write(fd, arr, size * sizeof(uint16_t));

	exit(0);
}
