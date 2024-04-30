#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <err.h>
#include <stdlib.h>

struct DataHeader {
	uint32_t magic;
	uint32_t count;
};

struct ComparatorHeader {
	uint32_t magic1;
	uint16_t magic2;

	uint16_t _;
	uint64_t count;
};

struct ComparatorSection {
	uint16_t type;

	uint16_t _;
	uint16_t __;
	uint16_t ___;

	uint32_t offset1;
	uint32_t offset2;
};

uint32_t getFileSize(const int fd, const char* file);
void writeNumOn(const uint32_t pos, const int fd, const uint64_t* num);
void readNumOn(const uint32_t pos, const int fd, uint64_t* num);
void lseekOn(const int fd, const uint32_t pos);

uint32_t getFileSize(const int fd, const char* file) {
	struct stat st;

	if(fstat(fd, &st) < 0) {
		err(3, "Error while fstating file %s", file);
	}

	return st.st_size;
}

void lseekOn(const int fd, const uint32_t pos) {
	const uint32_t sizeOfDataHeader = sizeof(struct DataHeader);
	const uint32_t sizeOfDataNumber = sizeof(uint64_t);

	int realPos = pos * sizeOfDataNumber + sizeOfDataHeader;

	if(lseek(fd, realPos, SEEK_SET) < 0) {
		err(19, "Error while lseeking in data file");
	}
}

void readNumOn(const uint32_t pos, const int fd, uint64_t* num) {
	lseekOn(fd, pos);

	if(read(fd, num, sizeof(uint64_t)) != sizeof(uint64_t)) {
		err(20, "Error while reading data for comparison in data file");
	}
}

void writeNumOn(const uint32_t pos, const int fd, const uint64_t* num) {
	lseekOn(fd, pos);

	if(write(fd, num, sizeof(uint64_t)) != sizeof(uint64_t)) {
		err(21, "Error while writng data for comparison in data file");
	}

}

int main(int argc, char** argv) {

	if(argc != 3) {
		errx(1, "Ivalid number of parameters");
	}

	// DATA
	int fddata = open(argv[1], O_RDWR);

	if(fddata < 0) {
		err(2, "Error while opening file %s", argv[1]);
	}

	uint32_t sizedata = getFileSize(fddata, argv[1]);

	if(sizedata < sizeof(struct DataHeader)) {
		errx(4, "File %s has no header", argv[1]);
	}

	struct DataHeader dataHeader;

	if(read(fddata, &dataHeader, sizeof(struct DataHeader)) != sizeof(struct DataHeader)) {
		err(5, "Error while reading header from file %s", argv[1]);
	}

	dprintf(1, "Data count: %d\n", dataHeader.count);

	if(dataHeader.magic != 0x21796F4A) {
		errx(6, "Invalid magic number in file %s", argv[1]);
	}

	uint32_t sizedataNoHeader = sizedata - sizeof(struct DataHeader);
	if(sizedataNoHeader % sizeof(uint64_t) != 0) {
		errx(7, "Invalid data in file %s", argv[1]);
	}
		
	if(dataHeader.count != sizedataNoHeader / sizeof(uint64_t)) {
		errx(8, "Invalid count in header in file %s", argv[1]);
	}


	// COMPARATOR
	int fdcomp = open(argv[2], O_RDONLY);

	if(fdcomp < 0) {
		err(9, "Error while opening file %s", argv[2]);
	}
	
	uint32_t sizecomp = getFileSize(fdcomp, argv[2]);

	if(sizecomp < sizeof(struct ComparatorHeader)) {
		errx(10, "File %s has no header", argv[2]);
	}

	struct ComparatorHeader comparatorHeader;

	if(read(fdcomp, &comparatorHeader, sizeof(struct ComparatorHeader)) != sizeof(struct ComparatorHeader)) {
		err(11, "Error while reading header from file %s", argv[2]);
	}

	dprintf(1, "Comparators count: %ld\n\n", comparatorHeader.count);

	if(comparatorHeader.magic1 != 0xAFBC7A37 || comparatorHeader.magic2 != 0x1C27) {
		errx(12, "Invalid magic number in file %s", argv[2]);
	}

	uint32_t sizecompNoHeader = sizecomp - sizeof(struct ComparatorHeader);
	if(sizecompNoHeader % sizeof(struct ComparatorSection) != 0) {
		errx(13, "Invalid data in file %s", argv[2]);
	}
		
	if(comparatorHeader.count != sizecompNoHeader / sizeof(struct ComparatorHeader)) {
		errx(14, "Invalid count in header in file %s", argv[2]);
	}


	//READING
	struct ComparatorSection s;

	uint64_t left [1];
	uint64_t right [1];

	for(uint64_t i = 0; i < comparatorHeader.count ; i++) {

		if(read(fdcomp, &s, sizeof(struct ComparatorSection)) != sizeof(struct ComparatorSection)) {
			err(15, "Error while reading from file %s", argv[2]);
		}

		if(s._ != 0 || s.__ != 0 || s.___ != 0) {
			errx(16, "Error in section data reserved value");
		}

		if(s.type != 0 && s.type != 1) {
			errx(17, "Error in section data type value");
		}

		if(s.offset1 > dataHeader.count || s.offset2 > dataHeader.count) {
			errx(18, "Error in section data offset value is larger than count");
		}

		dprintf(1, "Positions are %d and %d\n", s.offset1, s.offset2);
		readNumOn(s.offset1, fddata, left);
		readNumOn(s.offset2, fddata, right);

		dprintf(1, "Nums are %ld and %ld, Comparator is %d\n", *left, *right, s.type);

		if(*left > *right && s.type == 0) {
			dprintf(1, "Switching1 %ld and %ld\n", *left, *right);

			writeNumOn(s.offset1, fddata, right);
			writeNumOn(s.offset2, fddata, left);
		}
		else if(*left < *right && s.type == 1) {
			dprintf(1, "Switching2 %ld and %ld\n", *left, *right);

			writeNumOn(s.offset1, fddata, right);
			writeNumOn(s.offset2, fddata, left);
		}
		dprintf(1, "\n");
	}

	close(fddata);
	close(fdcomp);
	
	exit(0);
}
