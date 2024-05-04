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

const uint32_t MAGIC_NUMBER = 0xEFBEADDE;

struct Header {
	uint32_t magic;
	uint8_t headerVersion;

	uint8_t dataVersion;
	uint16_t count;
	uint32_t _;
	uint32_t __;
};

struct Data0x00 {
	uint16_t offset;
	uint8_t original;
	uint8_t replacer;
};

struct Data0x01 {
	uint32_t offset;
	uint16_t original;
	uint16_t replacer;
};
	
int openWrite(const char* file) {
	int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);

	if(fd < 0) {
		err(2, "Error while opening file %s", file);
	}

	return fd;
}

int main(int argc, char** argv) {

	int fd = openWrite(argv[1]);
	struct Header h;

	h.magic = 0xEFBEADDE;
	h.headerVersion = 0x01;
	h.dataVersion = 0x01;
	h.count=3;
	h._ = 0;
	h.__ = 0;

	if(write(fd, &h, sizeof(struct Header)) < 0) {
		err(4, "Error while reading header of file %s", argv[1]);
	}	       
	
	struct Data0x01 arr [3];

	for(int i = 1; i <= h.count; i++) {
		arr[i-1].offset = 2*i;
		arr[i-1].original = 2*i;
		arr[i-1].replacer= 0xAAAA;
	}

	write(fd, arr, h.count * sizeof(struct Data0x01));

	exit(0);
}
