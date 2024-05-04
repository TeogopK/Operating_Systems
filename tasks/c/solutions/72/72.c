#include <stdint.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

const int BYTE_TO_BITS = 8;

int openWrite(const char* file);
int openRead(const char* file);
uint32_t getFileSize(const int fd, const char* file);

void getBits(const uint8_t byte, bool* bits);
bool getBitOnPosition(const uint8_t byte, const int position);
void setBits(uint16_t* encoded, const int pos, const uint8_t bit);

void readByte(const int fd, const char* file, uint8_t* byte);
void write2Bytes(const int fd, const char* file, uint16_t* bytes);

void encodeManchester(const int infd, const char* infile, const int outfd, const char* outfile, const uint32_t count);

int openRead(const char* file) {
	int fd = open(file, O_RDONLY);

	if(fd < 0) {
		err(2, "Error while opening file %s",  file);
	}

	return fd;
}

int openWrite(const char* file) {
	int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);

	if(fd < 0) {
		err(2, "Error while opening file %s",  file);
	}

	return fd;
}

uint32_t getFileSize(const int fd, const char* file) {
	struct stat st;

	if(fstat(fd, &st) < 0) {
		err(3, "Error while fstating file %s", file);
	}

	return st.st_size;
}

bool getBitOnPosition(const uint8_t byte, const int position) {

	return (byte & (1 << position)) != 0;		
}

void getBits(const uint8_t byte, bool* bits) {

	int position;
	for(int i = 0; i < BYTE_TO_BITS; i++) {

		// bits[0] is the oldest (highest) bit
		position = BYTE_TO_BITS - 1 - i;
		bits[i] = getBitOnPosition(byte, position);
	}
}


void setBits(uint16_t* encoded, const int pos, const uint8_t bit) {
	int revPos = 2 * BYTE_TO_BITS - 1 - pos;
	*encoded = (*encoded) ^ (bit << revPos);
}

void readByte(const int fd, const char* file, uint8_t* byte) {
	if(read(fd, byte, sizeof(uint8_t)) < 0) {
		err(5, "Error while reading from file %s", file);
	}
}

void write2Bytes(const int fd, const char* file, uint16_t* bytes) {
	if(write(fd, bytes, sizeof(uint16_t)) < 0) {
		err(6, "Error while writing from file %s", file);
	}
}

void encodeManchester(const int infd, const char* infile, const int outfd, const char* outfile, const uint32_t count) {
	bool bits[BYTE_TO_BITS];

	uint8_t byte [1];

	for(uint32_t b = 0; b < count; b++) {

		uint16_t zero = 0;
		uint16_t* encoded = &zero;

		readByte(infd, infile, byte);
		getBits(*byte, bits);

		for(int i = 0; i < BYTE_TO_BITS; i++) {

			if(bits[i]) {
				setBits(encoded, 2 * i, 1);
				setBits(encoded, 2 * i + 1, 0);
			}
			else {
				setBits(encoded, 2 * i, 0);
				setBits(encoded, 2 * i + 1, 1);
			}

		}

		write2Bytes(outfd, outfile, encoded);
	}
}

int main(int argc, char** argv) {

	if(argc != 3) {
		errx(1, "Invalid number of parameters");
	}

	int infd = openRead(argv[1]);
	uint32_t count = getFileSize(infd, argv[1]);

	int outfd = openWrite(argv[2]);

	encodeManchester(infd, argv[1], outfd, argv[2], count);

	//closing
	close(infd);
	close(outfd);

	exit(0);
}
