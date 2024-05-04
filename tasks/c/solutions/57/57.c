#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

//add auto size
struct Pair{
	uint32_t position;
	uint32_t count;
};

int writeOpen(char* file);
int readOpen(char* file);
int checkSize(int fd, char* file, int mult);

int readOpen(char* file) {
	
	int fd = open(file, O_RDONLY);
	
	if(fd < 0) {
		err(2, "Error while opening file %s", file);
	}

	return fd;
}

int writeOpen(char* file) {
	
	int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	
	if(fd < 0) {
		err(2, "Error while opening file %s", file);
	}

	return fd;
}

int checkSize(int fd, char* file, int mult) {
	struct stat st;

	if(fstat(fd, &st) < 0) {
		err(3, "Failed to fstat file %s", file);

	}

	if(st.st_size % (mult * sizeof(uint32_t)) != 0) {
		errx(4, "Error, file %s is of invalid size", file);
	}

	return st.st_size;
}


int main(int argc, char** argv) {

	if(argc != 4) {
		errx(1, "Invalid number of parameters");
	}

	int fd1 = readOpen(argv[1]);	
	int fd2 = readOpen(argv[2]);	
	int fd3 = writeOpen(argv[3]);

	checkSize(fd1, argv[1], 2);	
	int size2 = checkSize(fd2, argv[2], 1);	

	int read_count = 0;
	struct Pair pair;

	while( (read_count = read(fd1, &pair, sizeof(struct Pair))) > 0) {

		int lseek_pos = pair.position * sizeof(uint32_t);
		int lseek_end_pos = (pair.count + pair.position) * sizeof(uint32_t);

		if(lseek_pos > size2 || lseek_end_pos > size2) {
			errx(6, "Invalid position to read from file %s", argv[2]);
		}

		if(lseek(fd2, lseek_pos, SEEK_SET) < 0) {
			err(7, "Error while lseeking in file %s", argv[2]);
		}

		uint32_t num;
		for(uint32_t i = 0; i < pair.count; i++) {

			if(read(fd2, &num, sizeof(uint32_t)) < 0) {
				err(8, "Error while reading from file %s", argv[2]);
			}

			if(write(fd3, &num, sizeof(uint32_t)) < 0) {
				err(9, "Error while writing to file %s", argv[3]);
			}
		}	

	}

	if(read_count < 0) {
		err(5, "Error while reading from file %s", argv[1]);
	}

	close(fd1);
	close(fd2);
	close(fd3);

	exit(0);
}
