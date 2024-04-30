#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <err.h>
#include <stdlib.h>

int main(int argc, char** argv) {

	int fd = open(argv[1], O_RDWR);


	uint8_t num =0xaa;

	for(int i = 1; i < 10; i++) {
	lseek(fd,i * 10, SEEK_SET);
		num += 1;
		write(fd, &num, sizeof(num));	
	}

	close(fd);

	exit(0);
}
