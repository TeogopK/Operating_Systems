#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <err.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>

int MAX_BUFF_SIZE = 1024;

struct triplet {
	uint16_t offset;
	uint8_t original;
	uint8_t replacer;
};

int openRead(const char* file);
int openWrite(const char* file);
int getFileSize(const int fd, const char* file);

void loadData(const int fd, const char* file, uint8_t* buff);
void patchData(const int patch_fd, const char* patch_file, uint8_t* buff, const int origin_size);
void saveData(const int fd, const char* file, uint8_t* buff, const int length);

int openRead(const char* file) {
	int fd = open(file, O_RDONLY);

	if(fd < 0) {
		err(2, "Error while opening file %s", file);
	}

	return fd;
}

int openWrite(const char* file) {
	int fd = open(file, O_CREAT | O_TRUNC | O_WRONLY, 0666);

	if(fd < 0) {
		err(2, "Error while opening file %s", file);
	}

	return fd;
}

int getFileSize(const int fd, const char* file) {
	struct stat st;

	if(fstat(fd, &st) < 0) {
		err(3, "Error while getting size of file %s", file);
	}

	return st.st_size;
}

void loadData(const int fd, const char* file, uint8_t* buff) {
	int read_bytes = 0;
	uint8_t symb;

	int i = 0;
	while( (read_bytes = read(fd, &symb, sizeof(symb))) > 0) {
		buff[i] = symb;
		i++;

		if(i >= MAX_BUFF_SIZE) {
			errx(6, "Error buffer overflow while reading from file %s", file);
		}
	}

	if(read_bytes < 0) {
		err(5, "Error while reading from file %s", file);
	}
}

void patchData(const int patch_fd, const char* patch_file, uint8_t* buff, const int origin_size) {
	int read_bytes = 0;
	struct triplet tr;

	while( (read_bytes = read(patch_fd, &tr, sizeof(struct triplet))) > 0) {
		if(tr.offset >= origin_size) {
			errx(10, "Invalid offset in patch entry in file %s", patch_file);
		}	

		if(buff[tr.offset] != tr.original) {
			errx(11, "Invalid original byte in patch entry in file %s", patch_file);
		}

		buff[tr.offset] = tr.replacer;
	}

	if(read_bytes < 0) {
		err(5, "Error while reading from file %s", patch_file);
	}
}

void saveData(const int fd, const char* file, uint8_t* buff, const int length) {

	if(write(fd, buff, length * sizeof(uint8_t)) < 0) {
		err(9, "Error while writing to file %s", file);
	}
}

int main(int argc, char** argv) {

	if(argc != 4) {
		warnx("Usage is %s <patch.bin> <f1.bin> <f2.bin>", argv[0]);
		errx(1, "Invalid number of parameters");
	}

	int patch_fd = openRead(argv[1]);
	int patch_size = getFileSize(patch_fd, argv[1]);

	if(patch_size % sizeof(struct triplet) != 0) {
		errx(4, "Invalid size of patch file %s", argv[1]);
	}

	int origin_fd = openRead(argv[2]);
	int origin_size = getFileSize(origin_fd, argv[2]);

	uint8_t buff [MAX_BUFF_SIZE];
	// maybe use malloc??	uint8_t* buff = (uint8_t*)malloc(sizeof(uint8_t) * origin_size);

	loadData(origin_fd, argv[2], buff);
	patchData(patch_fd, argv[1], buff, origin_size);

	int save_fd = openWrite(argv[3]);
	saveData(save_fd, argv[3], buff, origin_size);


	close(patch_fd);
	close(origin_fd);

	exit(0);
}

