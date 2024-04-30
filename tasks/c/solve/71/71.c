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
uint32_t getCountNumbers(const int fd, const char* file);

void getBits(const uint8_t byte, bool* bits);
bool getBitOnPosition(const uint8_t byte, const int position);

void saveNum(const int fd, const char* file, uint16_t* num);
void readNumFromData(const int fd, const char* file, uint16_t* num);
void readByteOfSequence(const int fd, const char* file, uint8_t* byte);

void saveNumbersOnUppedBits(const int seq_fd, const char* seq_file, const int data_fd, const char* data_file, const int out_fd, const char* out_file, const uint32_t count);

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

uint32_t getCountNumbers(const int fd, const char* file) {

	uint32_t fileSize = getFileSize(fd, file);

	if(fileSize % sizeof(uint16_t) != 0) {
		errx(4, "Invalid data in file %s", file);
	}

	return fileSize / sizeof(uint16_t);
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

		dprintf(1, "%d ", bits[i]);
	}

	dprintf(1, " | ");
}

void readByteOfSequence(const int fd, const char* file, uint8_t* byte) {
	if(read(fd, byte, sizeof(uint8_t)) < 0) {
		err(5, "Error while reading from file %s", file);
	}
}


void readNumFromData(const int fd, const char* file, uint16_t* num) {
	if(read(fd, num, sizeof(uint16_t)) < 0) {
		err(5, "Error while reading from file %s", file);
	}
}


void saveNum(const int fd, const char* file, uint16_t* num) {
	if(write(fd, num, sizeof(uint16_t)) < 0) {
		err(6, "Error while writing from file %s", file);
	}
}

void saveNumbersOnUppedBits(const int seq_fd, const char* seq_file, const int data_fd, const char* data_file, const int out_fd, const char* out_file, const uint32_t count) {

	bool bits[BYTE_TO_BITS];

	uint8_t byte [1];
	uint16_t num [1];

	for(uint32_t s = 0; s < count; s++) {

		readByteOfSequence(seq_fd, seq_file, byte);
		getBits(*byte, bits);

		for(int i = 0; i < BYTE_TO_BITS; i++) {

			readNumFromData(data_fd, data_file, num);

			if(bits[i]) {
				saveNum(out_fd, out_file, num);
			}

		}

	}

	dprintf(1, "\n");
}

int main(int argc, char** argv) {

	if(argc != 4) {
		errx(1, "Invalid number of parameters");
	}

	int seq_fd = openRead(argv[1]);
	int data_fd = openRead(argv[2]);

	uint32_t countNumbers = getCountNumbers(data_fd, argv[2]);

	uint32_t filesizeSeq = getFileSize(seq_fd, argv[1]);
	if(countNumbers != filesizeSeq * BYTE_TO_BITS)  {

		errx(4, "Count of numbers in data file %s does not equal the count of bits in sequence file %s", argv[2], argv[1]);
	}

	int out_fd = openWrite(argv[3]);

	saveNumbersOnUppedBits(seq_fd, argv[1], data_fd, argv[2], out_fd, argv[3], filesizeSeq);


	//closing
	close(seq_fd);
	close(data_fd);
	close(out_fd);

	exit(0);
}
