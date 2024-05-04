#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>

struct entry {
	uint32_t uid;
	uint16_t _;
	uint16_t __;
	uint32_t start;
	uint32_t end;
};


int openFile(const char* file);

int openFile(const char* file) {
	int fd = open(file, O_CREAT | O_APPEND | O_RDWR, 0666);

	if(fd < 0) {
		err(2, "Error while openinig file %s", file);
	}

	return fd;
}

const int size = 5;
int main(int argc, char** argv) {

	int fd = openFile(argv[1]);

	struct entry arr[size];

	for(int i = 0; i < size; i++) {
		arr[i].uid = 1 + i;
		arr[i]._ = 0;
		arr[i]._ = 0;
		arr[i].start = 1 + 2*i;
		arr[i].end =1 + 4*i;

	}

	if(write(fd, arr, sizeof(struct entry) * size) < 0) {
		err(3, "Error while writing");
	}

	close(fd);

	exit(0);
}
