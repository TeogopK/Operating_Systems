#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <err.h>

int main(int argc, char** argv) {

	if(argc != 2) {
		errx(1, "ERROR: Arg count");    
	}

	uint32_t bytes[256];

	for(int i = 0; i < 256; i++) {
		bytes[i] = 0;
	}

	int fd;
	if(( fd = open(argv[1], O_RDWR)) == -1 ) {
		err(2, "ERROR: Opening file %s", argv[1]);
	}

	uint8_t buf;

	int bytes_count;
	while(( bytes_count = read(fd, &buf, sizeof(buf))) > 0) {
		bytes[buf] += 1;
	}

	if(bytes_count == -1) {
		err(2, "ERROR: Reading from file");
	}    

	if(lseek(fd, 0, SEEK_SET) == -1) {
		err(3, "ERROR: Could not lseek");
	}

	for(int i = 0; i < 256; i++) {
		uint8_t temp = i;
		for(uint32_t j = 0; j < bytes[i]; j++) {
			if(write(fd, &temp, sizeof(temp)) == -1) {
				err(4, "ERROR: Writing to file");
			}
		}
	}

	close(fd);

	return 0;
}

