#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <err.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>

struct triplet {
	uint16_t off;
	uint8_t original;
	uint8_t replacer;
};

int main(int argc, char** argv) {

	struct triplet tr;

	tr.off = 0x0003;
	tr.original=0x59;
	tr.replacer=0x60;

	int fd = open("patch.bin", O_CREAT | O_TRUNC | O_WRONLY, 0666);

	write(fd, &tr, sizeof(struct triplet));


	tr.off = 0x0030;
	tr.original=0x35;
	tr.replacer=0xaf;
	write(fd, &tr, sizeof(struct triplet));

	tr.off = 0x0030;
	tr.original=0xaf;
	tr.replacer=0x77;
	write(fd, &tr, sizeof(struct triplet));

	tr.off = 0x0001;
	tr.original=0x2e;
	tr.replacer=0xaa;
	write(fd, &tr, sizeof(struct triplet));

	close(fd);
	exit(0);
}

