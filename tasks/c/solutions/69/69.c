#include <err.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

const uint32_t MAGIC_NUMBER = 0xEFBEADDE;

struct Header {
	uint32_t magic;
	uint8_t headerVersion;

	uint8_t dataVersion;
	uint16_t count;
	uint32_t _;
	uint32_t __;
};

struct Data0x00 {
	uint16_t offset;
	uint8_t original;
	uint8_t replacer;
};

struct Data0x01 {
	uint32_t offset;
	uint16_t original;
	uint16_t replacer;
};

int openRead(const char* file);
int openWrite(const char* file);
uint32_t getFileSize(const int fd, const char* file);

struct Header readHeader(const int fd, const char* file);
int getSizeDataPatch(const uint8_t version);
int getSizeNormalData(const uint8_t version);

void checkHeader(const struct Header h, const char* file);
void checkPatchSize(const uint32_t size, const int sizePatchData, const uint16_t expectedCount, const char* file);
void checkDataSize(const uint32_t size, const int sizeData, const char* file);

void copyData(const int fd_in, const char* file_in, const int fd_out, const char* file_out, const int sizeData);
void writeData(const int fd, const char* file, const uint16_t num, const int size);

void lseekOnPosition(const int fd, const char* file, const uint16_t pos);
void replaceData(const int fd_patch, const char* file_patch, const uint32_t sizeDataFile, const int fd_out, const char* file_out, const int sizeNormalData, const uint16_t count);

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

struct Header readHeader(const int fd, const char* file) {

	struct Header h;
	if(read(fd, &h, sizeof(struct Header)) < 0) {
		err(4, "Error while reading header of file %s", file);
	}	       

	dprintf(0, "%d Version\n", h.dataVersion);
	dprintf(0, "%d Count\n", h.count);

	return h;	
}

void checkPatchSize(const uint32_t size, const int sizePatchData, const uint16_t expectedCount, const char* file) {

	int checkSize = size - sizeof(struct Header);

	if(checkSize % sizePatchData != 0) {
		errx(12, "Patch file %s contains incorrect data", file);
	}

	if(checkSize / sizePatchData != expectedCount) {
		errx(12, "Patch file %s contains incorrect data", file);
	}
}

void checkDataSize(const uint32_t size, const int sizeData, const char* file) {

	if(size % sizeof(sizeData) != 0) {
		errx(14, "Data file %s contains incorrect data", file);
	}
}

void checkHeader(const struct Header h, const char* file) {

	if(h.magic != MAGIC_NUMBER) {
		errx(19, "Invalid magic number in patch file %s", file);
	}

	if(h.headerVersion != 0x01) {
		errx(11, "Invalid header version of patch file %s", file);
	}
	if(h.dataVersion != 0x00 && h.dataVersion != 0x01) {
		errx(18, "Patch file %s has incorrect data version", file);
	}
}

int getSizeNormalData(const uint8_t version)
{
	if(version == 0x00) {
		return sizeof(uint8_t);
	}

	else if(version == 0x01) {
		return sizeof(uint16_t);
	}

	return 0;
}

int getSizeDataPatch(const uint8_t version)
{
	if(version == 0x00) {
		return sizeof(struct Data0x00);
	}

	else if(version == 0x01) {
		return sizeof(struct Data0x01);
	}

	return 0;
}

void copyData(const int fd_in, const char* file_in, const int fd_out, const char* file_out, const int sizeData) {

	int32_t read_count = 0;
	uint16_t num;

	while( (read_count = read(fd_in, &num, sizeData)) > 0) {
		writeData(fd_out, file_out, num, sizeData);
	}

	if(read_count < 0) {
		err(20, "Error while reading data from file %s", file_in);
	}
}

void writeData(const int fd, const char* file, const uint16_t num, const int size) {

	if(write(fd, &num, size) < 0){
		err(21, "Error while writing to file %s", file);
	}
}

void lseekOnPosition(const int fd, const char* file, const uint16_t pos) {

	if(lseek(fd, pos, SEEK_SET) < 0) {
		err(22, "Error while lseeking in file %s", file);
	}
}

void replaceData(const int fd_patch, const char* file_patch, const uint32_t sizeDataFile, const int fd_out, const char* file_out, const int sizeNormalData, const uint16_t count)
{
	uint32_t maxLseek = sizeDataFile / sizeNormalData;

	uint32_t offset;
	uint32_t original;
	uint32_t replacer;

	int sizes[3];
	//some magic number and calculations, because sizeNormalData is 1 or 2
	sizes[0] = 2 * sizeNormalData;
	sizes[1] = 1 * sizeNormalData;
	sizes[2] = 1 * sizeNormalData;

	uint32_t* ptrs[3];
	ptrs[0] = &offset;
	ptrs[1] = &original;
	ptrs[2] = &replacer;

	for(int e = 0; e < count; e++) {
		for(int i = 0; i < 3; i++) {
			if(read(fd_patch, ptrs[i] , sizes[i]) < 0) {
				err(22, "Error while reading data from file %s", file_patch);
			}
			dprintf(1, "MAGIC %d: %d\n", i, *ptrs[i]);
		}

		if(offset > maxLseek) {
			errx(23, "Error in patch file %s entry offset", file_patch);
		}

		lseekOnPosition(fd_out, file_out, offset * sizeNormalData);

		uint32_t num;
		if(read(fd_out, &num, sizeNormalData) < 0) {
			err(24, "Error while reading data from file %s", file_out);
		}
			dprintf(1, "ORIGINAL %d: %d\n", e, num);

		if(num != original) {
			errx(23, "Error in patch file %s entry original word", file_patch);
		}

		lseekOnPosition(fd_out, file_out, offset * sizeNormalData);

		writeData(fd_out, file_out, (uint16_t) replacer, sizeNormalData);
	}
}

int main(int argc, char** argv) {

	if(argc != 4) {
		errx(1, "Invalid number of parameters");
	}

	//header
	int fd_patch = openRead(argv[1]);

	uint32_t sizePatchFile = getFileSize(fd_patch, argv[1]);

	if(sizePatchFile < sizeof(struct Header)) {
		errx(10, "Invalid patch file %s: Does not contain header", argv[1]);
	}

	struct Header h = readHeader(fd_patch, argv[1]);

	checkHeader(h, argv[1]);

	uint8_t version = h.dataVersion;
	int sizeDataPatch = getSizeDataPatch(version);
	int sizeNormalData = getSizeNormalData(version);

	dprintf(1, "%d Patch\n", sizeDataPatch);
	dprintf(1, "%d Normal\n", sizeNormalData);
	checkPatchSize(sizePatchFile, sizeDataPatch, h.count, argv[1]);

	//data
	int fd_data = openRead(argv[2]);
	uint32_t sizeDataFile = getFileSize(fd_data, argv[2]);

	checkDataSize(sizeDataFile, sizeNormalData, argv[2]);


	int fd_out = openWrite(argv[3]);
	copyData(fd_data, argv[2], fd_out, argv[3], sizeNormalData);


	replaceData(fd_patch, argv[1], sizeDataFile, fd_out, argv[3], sizeNormalData, h.count);

	//closing
	close(fd_out);
	close(fd_data);
	close(fd_patch);

	exit(0);
}
