#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>

const uint32_t MAX_SIZE = 200;

int openWrite(const char* file);
int openRead(const char* file);

uint32_t getFileSize(const int fd, const char* file);
void lseekInBeginning(const int fd, const char* file);

void loadHalfData(const int fd, const char* file, uint32_t* arr, const uint32_t count);

void sortHalfData(uint32_t* arr, const uint32_t count);

void saveHalfData(const int fd, const char* file, uint32_t* arr, const uint32_t count);

void mergeHalves(const int fd_output, const char* file_output, int* fds, const char** tempFiles, const uint32_t* lengths);

void writeOneNum(const int fd, const char* file, const uint32_t num);
uint32_t readOneNum(const int fd, const char* file);

int openRead(const char* file) {

	int fd = open(file, O_RDONLY);

	if(fd < 0) {
		err(2, "Error while opening file %s", file);
	}

	return fd;
}

int openWrite(const char* file) {

	int fd = open(file, O_RDWR | O_CREAT | O_TRUNC, 0666);

	if(fd < 0) {
		err(2, "Error while opening file %s", file);
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

void loadHalfData(const int fd, const char* file, uint32_t* arr, const uint32_t count) {
	int32_t read_count = 0;

	read_count = read(fd, arr, sizeof(uint32_t) * count);

	if(read_count < 0) {
		err(6, "Error while reading from file %s", file);
	}
}

void saveHalfData(const int fd, const char* file, uint32_t* arr, const uint32_t count) {
	if(write(fd, arr, sizeof(uint32_t) * count) < 0) {
		err(7, "Error while writing in file %s", file);
	}
}


void sortHalfData(uint32_t* arr, const uint32_t count) {

	uint32_t temp;
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

void lseekInBeginning(const int fd, const char* file) {

	if(lseek(fd, 0, SEEK_SET) < 0) {
		err(8, "Error while lseeking in file %s", file);
	}
}

void mergeHalves(const int fd_output, const char* file_output, int* fds, const char** tempFiles, const uint32_t* lengths) {

	uint32_t left, right;

	uint32_t i = 1;
	uint32_t j = 1;

	left = readOneNum(fds[0], tempFiles[0]);
	right = readOneNum(fds[1], tempFiles[1]);

	while(true) {
		if(left < right) {
			writeOneNum(fd_output, file_output, left);
			i++;

			if(i > lengths[0]) {
				break;
			}
			left = readOneNum(fds[0], tempFiles[0]);

		} else {
			writeOneNum(fd_output, file_output, right);
			j++;

			if(j > lengths[1]) {
				break;
			}
			right = readOneNum(fds[1], tempFiles[1]);
		}
	}

	while(i <= lengths[0]) {
		writeOneNum(fd_output, file_output, left);
		i++;

		if(i > lengths[0]) {
			break;
		}
		left = readOneNum(fds[0], tempFiles[0]);
	}

	while(j <= lengths[1]) {
		writeOneNum(fd_output, file_output, right);
		j++;

		if(j > lengths[1]) {
			break;
		}
		right = readOneNum(fds[1], tempFiles[1]);
	}
}

uint32_t readOneNum(const int fd, const char* file) {

	uint32_t symb;

	if(read(fd, &symb, sizeof(uint32_t)) < 0) {
		err(11, "Error while reading from file %s", file);
	}

	return symb;
}


void writeOneNum(const int fd, const char* file, const uint32_t num) {
	if(write(fd, &num, sizeof(uint32_t)) < 0) {
		err(12, "Error while writing to file %s", file);
	}
}

int main(int argc, char** argv) {

	if(argc != 3) {
		warnx("Program sorts uint32_t from input file and saves them in output file with RAM restriction");
		errx(1, "Usage is %s <input.file> <output.file>", argv[0]);
	}

	int fd_input = openRead(argv[1]);
	int size = getFileSize(fd_input, argv[1]);

	if(size % sizeof(uint32_t) != 0) {
		errx(4, "Incorrect data in file %s", argv[1]);
	}

	uint32_t length = size / sizeof(uint32_t);	

	uint32_t arr[MAX_SIZE / 2];

	if(length > MAX_SIZE) {
		errx(5, "Data in file %s is too big to be loaded in parts of 2", argv[1]);
	}	

	uint32_t lengths[2]; 
	lengths[0] = length / 2;
	lengths[1] = length - lengths[0];

	int fds[2];
	const char* tempFiles[2] = {"file1.bin", "file2.bin"};

	for(int i = 0; i < 2; i++) {
		loadHalfData(fd_input, argv[1], arr, lengths[i]);

		fds[i] = openWrite(tempFiles[i]);

		sortHalfData(arr, lengths[i]);
		saveHalfData(fds[i], tempFiles[i], arr, lengths[i]);

		lseekInBeginning(fds[i], tempFiles[i]);
	}

	int fd_output = openWrite(argv[2]);
	mergeHalves(fd_output, argv[2], fds, tempFiles, lengths);

	close(fd_output);
	close(fd_input);
	close(fds[0]);
	close(fds[1]);

	exit(0);
}

