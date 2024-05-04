#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <err.h>
#include <stdlib.h>

struct __attribute__ ((__packed__)) Foo {
	uint32_t a;
	uint8_t b;
	char c;
	uint16_t d;
	char e;
};

int main(int argc, char** argv) {

	dprintf(1, "%ld", sizeof(struct Foo));		

	exit(0);
}
