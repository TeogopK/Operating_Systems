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

const int TWO_BYTES_TO_BITS = 16;

int openWrite(const char* file);
int openRead(const char* file);
uint32_t getFileSize(const int fd, const char* file);

void getBits(const uint16_t byte, bool* bits);
bool getBitOnPosition(const uint16_t byte, const int position);
void setBits(uint8_t* decoded, const int pos, const uint8_t bit);

void read2Bytes(const int fd, const char* file, uint16_t* bytes);
void writeByte(const int fd, const char* file, uint8_t* byte);

void decodeManchester(const int infd, const char* infile, const int outfd, const char* outfile, const uint32_t count);

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

bool getBitOnPosition(const uint16_t bytes, const int position) {

	return (bytes & (1 << position)) != 0;		
}

void getBits(const uint16_t bytes, bool* bits) {

	int position;
	for(int i = 0; i < TWO_BYTES_TO_BITS; i++) {

		// bits[0] is the oldest (highest) bit
		position = TWO_BYTES_TO_BITS - 1 - i;
		bits[i] = getBitOnPosition(bytes, position);
	}
}


void setBits(uint8_t* decoded, const int pos, const uint8_t bit) {
	int revPos = TWO_BYTES_TO_BITS / 2 - 1 - pos;
	*decoded = (*decoded) ^ (bit << revPos);
}

void read2Bytes(const int fd, const char* file, uint16_t* bytes) {
	if(read(fd, bytes, sizeof(uint16_t)) < 0) {
		err(5, "Error while reading from file %s", file);
	}
}

void writeByte(const int fd, const char* file, uint8_t* byte) {
	if(write(fd, byte, sizeof(uint8_t)) < 0) {
		err(6, "Error while writing from file %s", file);
	}
}

void decodeManchester(const int infd, const char* infile, const int outfd, const char* outfile, const uint32_t count) {
	bool bits[TWO_BYTES_TO_BITS];

	uint16_t bytes [1];

	for(uint32_t b = 0; b < count; b++) {

		uint8_t zero = 0;
		uint8_t* decoded = &zero;

		read2Bytes(infd, infile, bytes);
		getBits(*bytes, bits);

		for(int i = 0; i < TWO_BYTES_TO_BITS; i += 2) {

			if(bits[i] && !bits[i + 1]) {
				setBits(decoded, i / 2, 1);
			}
			else if(!bits[i] && bits[i + 1]){
				setBits(decoded, i / 2, 0);
			}
			else{
				errx(10, "Incorrect input file %s", infile);
			}
		}

		writeByte(outfd, outfile, decoded);
	}
}

int main(int argc, char** argv) {

	if(argc != 3) {
		errx(1, "Invalid number of parameters");
	}

	int infd = openRead(argv[1]);
	uint32_t size = getFileSize(infd, argv[1]);

	if(size % sizeof(uint16_t) != 0) {
		errx(11, "Incorrect decoded data in file %s", argv[1]);
	}

	uint32_t count =  size / sizeof(uint16_t);

	int outfd = openWrite(argv[2]);

	decodeManchester(infd, argv[1], outfd, argv[2], count);

	//closing
	close(infd);
	close(outfd);

	exit(0);
}

