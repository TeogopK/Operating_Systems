#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>

int openRead(char* file);
int openWrite(char* file);
int getSize(int fd, char* file);

struct triplet {
	uint16_t index;
	uint8_t length;
	uint8_t _;
};

int getSize(int fd, char* file) {
	struct stat st;

	if(fstat(fd, &st) < 0) {
		err(3, "Error while getting size of file %s", file);
	}

	return st.st_size;	
}

int openWrite(char* file) {
	int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);

	if(fd < 0) { 
		err(2, "Error while opening file %s", file);
	}

	return fd;
}

int openRead(char* file) {
	int fd = open(file, O_RDONLY);

	if(fd < 0) { 
		err(2, "Error while opening file %s", file);
	}

	return fd;
}

int main(int argc, char ** argv) {

	if(argc != 5) {
		errx(1, "Invalid number of parameters");
	}

	int f1_dat = openRead(argv[1]);
	int f1_idx = openRead(argv[2]);

	int size_data = getSize(f1_dat, argv[1]);
	int size_f1_idx = getSize(f1_idx, argv[2]);

	if(size_f1_idx % sizeof(struct triplet) != 0) {
		errx(4, "Error file %s is of incorrect size", argv[2]);
	}

	int f2_dat = openWrite(argv[3]);
	int f2_idx = openWrite(argv[4]);

	struct triplet tr;

	int read_count = 0;

	struct triplet saved_tr;
	saved_tr.index = 0;
	saved_tr.length = 0;
	saved_tr._ = 0;

	while( (read_count = read(f1_idx, &tr, sizeof(struct triplet))) > 0) {

		int end_pos = tr.index + tr.length;
		if(size_data < tr.index || size_data < end_pos) {
			errx(6, "Error filedata in file %s is too small to read correctly", argv[1]);
		}

		if(lseek(f1_dat, tr.index, SEEK_SET) < 0) {
			err(7, "Error while lseeking in file %s", argv[1]);
		}

		uint8_t symb;
		int first_time = 0;

		for(int i = 0; i < tr.length; i++) {
			int read_data_count = 0;

			read_data_count = read(f1_dat, &symb, sizeof(uint8_t));

			if(read_data_count < 0) {
				err(8, "Error while reading data from file %s", argv[1]);
			}

			if(first_time == 0) {
				if(symb < 'A' || symb > 'Z') {
					break;
				}
				first_time = 1;
			}

			if(write(f2_dat, &symb, sizeof(uint8_t)) < 0) {
				err(9, "Error while writing to file %s", argv[3]);
			}
		}

		if(first_time == 1) {
			saved_tr.length = tr.length;

			if(write(f2_idx, &saved_tr, sizeof(struct triplet)) < 0) {
				err(9, "Error while writing to file %s", argv[4]);
			}

			saved_tr.index += tr.length;
		}
	}

	if(read_count < 0) {
		err(4, "Error while reading from file %s", argv[2]);
	}

	close(f1_dat);
	close(f1_idx);
	close(f2_dat);
	close(f2_idx);

	exit(0);
}
