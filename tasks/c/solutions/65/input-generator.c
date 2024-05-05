
#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char** argv) {

	const int size = 20;
	uint32_t arr[size];

	int fd = open("input.bin", O_WRONLY | O_CREAT | O_TRUNC, 0666);

	for(int i = 0; i < size / 2; i++) {
		arr[i] = (i + 1) * 2;
		arr[i + size/2] = (i + 1) * 2 + 1;
	}

	write(fd, arr, sizeof(uint32_t) * size);
}

