#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <err.h>
#include <stdlib.h>

struct ComparatorHeader {
	uint32_t magic1;
	uint16_t magic2;

	uint16_t _;
	uint64_t count;
};

struct ComparatorSection {
	uint16_t type;

	uint16_t _;
	uint16_t __;
	uint16_t ___;

	uint32_t offset1;
	uint32_t offset2;
};

int main (int argc, char** argv) {

	int fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
	struct ComparatorHeader ch;

	ch.magic1 = 0xAFBC7A37;
	ch.magic2 = 0x1C27;
	ch._ = 0;
	ch.count = 3;

	write(fd, &ch, sizeof(struct ComparatorHeader));

	struct ComparatorSection s;

	s._ = 0;
	s.__ = 0;
	s.___ = 0;
	
	s.type = 0;
	s.offset1 = 1;
	s.offset2 = 2;

	write(fd, &s, sizeof(struct ComparatorSection));

	//2nd
	s.type = 1;
	s.offset1 = 4;
	s.offset2 = 7;

	write(fd, &s, sizeof(struct ComparatorSection));

	//3rd
	s.type = 0;
	s.offset1 = 12;
	s.offset2 = 10;

	write(fd, &s, sizeof(struct ComparatorSection));

	

	exit(0);
}
