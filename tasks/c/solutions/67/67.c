#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <err.h>

uint32_t UPPER_BOUND_NUMS = 256000;

int openWrite(const char* file);
int openRead(const char* file);
uint32_t getFileSize(const int fd, const char* file);

void loadData(const int fd, const char* file, uint16_t* arr, const uint32_t size);
void sortData(uint16_t* arr, const uint32_t count);
void saveData(uint16_t* arr, const uint32_t size, const int fd, const char* file);

int openRead(const char* file) {
	int fd = open(file, O_RDONLY);

	if(fd < 0) {
		err(10, "Error while opening file %s", file);
	}

	return fd;
}

int openWrite(const char* file) {
	int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);

	if(fd < 0) {
		err(10, "Error while opening file %s", file);
	}

	return fd;
}

uint32_t getFileSize(const int fd, const char* file) {
	struct stat st;

	if(fstat(fd, &st) < 0) {
		err(11, "Error while fstating file %s", file);
	}

	return st.st_size;
}

void loadData(const int fd, const char* file, uint16_t* arr, const uint32_t size) {

	if(read(fd, arr, size) < 0) {
		err(12, "Error while reading from file %s", file);
	}
}

void sortData(uint16_t* arr, const uint32_t count) {

	uint16_t temp;
	for(uint32_t i = 0; i < count - 1; i++) {

		for(uint32_t j = i + 1; j < count; j++) {
			
			if(arr[i] > arr[j]) {
				temp = arr[i];
				arr[i] = arr[j];
				arr[j] = temp;
			}
		}
	}

}

void saveData(uint16_t* arr, const uint32_t size, const int fd, const char* file) {

	if(write(fd, arr, size) < 0) {
		err(13, "Error while writing to file %s", file);
	}
}

int main(int argc, char** argv) {

	if(argc != 3) {
		errx(1, "Ivalid number of parameters");
	}

	int fd_in = openRead(argv[1]);

	uint32_t size = getFileSize(fd_in, argv[1]);

	if(size % sizeof(uint16_t) != 0) {
		errx(2, "Invalid data in file %s", argv[1]);
	}	

	if(size > UPPER_BOUND_NUMS) {
		errx(2, "Data in file %s is too big to be loaded at once", argv[1]);
	}	

	uint32_t length = size / sizeof(uint16_t);

	uint16_t* arr = (uint16_t* )malloc(size);

	if(arr == NULL) {
		err(3, "Error while creating arr");
	}

	loadData(fd_in, argv[1], arr, size);
	sortData(arr, length);

	int fd_out = openWrite(argv[2]);
	saveData(arr, size, fd_out, argv[2]);


	free(arr);

	close(fd_in);
	close(fd_out);

	exit(0);
}
