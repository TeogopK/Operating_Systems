#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>

struct triplet {
	uint16_t index;
	uint8_t length;
	uint8_t _;
};

int main(void) {

	struct triplet tr;
	int fd = open("f1.idx", O_RDWR | O_CREAT | O_TRUNC, 0666);

	if(fd < 0) { 
		err(2, "Opening");
	}

	tr.index = 0;
	tr.length = 5;
	tr._ = 0;

	if(write(fd, &tr, sizeof(struct triplet)) < 0) {
		err(1, "Error writing");
	}

	tr.index = 0x014f;
	tr.length = 2;
	tr._ = 0;

	if(write(fd, &tr, sizeof(struct triplet)) < 0) {
		err(1, "Error writing");
	}
	
	tr.index = 0x014e;
	tr.length = 2;
	tr._ = 0;

	if(write(fd, &tr, sizeof(struct triplet)) < 0) {
		err(1, "Error writing");
	}

	tr.index = 0x014f;
	tr.length = 7;
	tr._ = 0;

	if(write(fd, &tr, sizeof(struct triplet)) < 0) {
		err(1, "Error writing");
	}
}
