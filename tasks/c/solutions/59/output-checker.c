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
	int fd = open("f2.idx", O_RDONLY, 0666);

	if(fd < 0) { 
		err(2, "Opening");
	}

	if(read(fd, &tr, sizeof(struct triplet)) < 0) {
		err(1, "Error writing");
	}
	if(read(fd, &tr, sizeof(struct triplet)) < 0) {
		err(1, "Error writing");
	}
	dprintf(1, "%d", tr.index);
}
