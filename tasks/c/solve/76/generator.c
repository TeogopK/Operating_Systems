#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <err.h>
#include <stdio.h>

struct DataHeader {
	uint32_t magic;
	uint32_t count;
};

int main(int argc, char** argv) {

	int fddata = open(argv[1], O_RDWR| O_CREAT| O_TRUNC, 0666);

	if(fddata < 0) {
		err(2, "Error while opening file %s", argv[1]);
	}

	struct DataHeader dataHeader;

	dataHeader.magic = 0x21796F4A;
	dataHeader.count = 20;

	if(write(fddata, &dataHeader, sizeof(struct DataHeader)) != sizeof(struct DataHeader)) {
		err(5, "Error while writing header from file %s", argv[1]);
	}

	uint64_t arr [20];

	for(int i = 0; i < 20; i++) {
		arr[i] = i;
	}	

	write(fddata, arr, sizeof(uint64_t) * 20);
	
	exit(0);
}
