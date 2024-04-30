#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

int openRead(char* file);
int openWrite(char* file);
int getFileSize(int fd, char* file);
uint8_t readByte(int fd, char* file);

struct triplet{
	uint16_t pos;
	uint8_t left;
	uint8_t right;
};

int openRead(char* file) {
	int fd = open(file, O_RDONLY);

	if(fd < 0) {
		err(2, "Error failed to open file %s", file);
	}

	return fd;
}

int openWrite(char* file) {
	int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);

	if(fd < 0) {
		err(2, "Error failed to open file %s", file);
	}

	return fd;
}

int getFileSize(int fd, char* file) {
	struct stat st;

	if(fstat(fd, &st) < 0) {
		err(3, "Error failed to get size of file %s", file);
	}

	return st.st_size;
}

uint8_t readByte(int fd, char* file) {
	uint8_t symb;

	if(read(fd, &symb, sizeof(uint8_t)) < 0) {
		err(5, "Error while reading from file %s", file);
	}

	return symb;
}

int main(int argc, char** argv) {

	if(argc != 4) {
		errx(1, "Invalid number of parameters");
	}

	int fd1 = openRead(argv[1]);
	int fd2 = openRead(argv[2]);

	int size1 = getFileSize(fd1, argv[1]);

	if(size1 % sizeof(struct triplet) != 0) {
		errx(4, "File %s is of invalid size", argv[1]);
	}

	int size2 = getFileSize(fd2, argv[2]);
	if(size2 != size1) {
		errx(4, "File %s is of different size compared to %s", argv[2], argv[1]);
	}

	int fdOut = openWrite(argv[3]);

	uint8_t symb1;
	uint8_t symb2;
	struct triplet tr;

	for(int i = 0; i < size1; i++) {

		symb1 = readByte(fd1, argv[1]);
		symb2 = readByte(fd2, argv[2]);

		if(symb1 != symb2) {
			tr.pos = i;
			tr.left = symb1;
			tr.right = symb2;

			if(write(fdOut, &tr, sizeof(struct triplet)) < 0) {
				err(6, "Error while writing to file %s", argv[3]);
			}
		}
	}

	close(fd1);
	close(fd2);
	close(fdOut);

	exit(0);
}
