#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>

struct entry {
	uint32_t uid;
	uint16_t _;
	uint16_t __;
	uint32_t start;
	uint32_t end;
};

struct userTime {
 	uint32_t uid;
	uint32_t time;
};

const uint32_t MAX_ENTRIES = 16384;
const uint32_t MAX_UNIQUE_ENTRIES = 2048;

int openFile(const char* file);
uint32_t getFileSize(const int fd, const char* file);

void loadEntries(const int fd, const char* file, struct entry* arr);
double getAverage(struct entry* entries, const uint32_t count);
double getDispersion(struct entry* entries, const uint32_t count, const double average);
uint32_t loadMaxPerUID(struct entry* entries, const uint32_t count, struct userTime* times);

void print(const double dispersion, struct userTime* times, const uint32_t count);
void writeUserTime(const struct userTime aUserTime);

int openFile(const char* file) {
	int fd = open(file, O_RDONLY);

	if(fd < 0) {
		err(2, "Error while openinig file %s", file);
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

void loadEntries(const int fd, const char* file, struct entry* arr) {
	int32_t read_count = 0;

	uint32_t i = 0;
	while( (read_count = read(fd, arr + i, sizeof(struct entry))) > 0) {
		i++;

		if(arr[i].start > arr[i].end) {
			errx(6, "File %s has invalid entry", file);
		}
	}

	if(read_count < 0) {
		err(5, "Error while reading entries from file %s", file);
	}
}

double getAverage(struct entry* entries, const uint32_t count) {

	double avg = 0;
	for(uint32_t i = 0; i < count; i++) {
		avg += (entries[i].end - entries[i].start);
	}
	return avg / count;
}

double getDispersion(struct entry* entries, const uint32_t count, const double average) {

	double disp = 0;
	double pow2 = 0;
	for(uint32_t i = 0; i < count; i++) {
		pow2 = (entries[i].end - entries[i].start) - average;
		pow2 *= pow2;
		
		disp += pow2;
	}
	
	return disp / count;

}

uint32_t loadMaxPerUID(struct entry* entries, const uint32_t count, struct userTime* times) {

	uint32_t newSize = 0;
	struct entry temp;
	uint32_t tempTime;
	
	for(uint32_t i = 0; i < count; i++) {

		bool isNew = true;
		temp = entries[i];
		tempTime = temp.end - temp.start;

		for(uint32_t j = 0; j < newSize; j++) {

			if(times[j].uid == temp.uid) {
				isNew = false;

				if(times[j].time < tempTime) {
					times[j].time = tempTime;
				}

				break;
			}
		}

		if(isNew) {
			times[newSize].uid = temp.uid;
			times[newSize].time = tempTime;
			newSize++;

			if(newSize > MAX_UNIQUE_ENTRIES) {
				errx(10, "Too many unique entries");
			}
		}
	}
	return newSize;
}

void writeUserTime(const struct userTime aUserTime) {
	if(dprintf(1, "User: %d; Max spend time: %d\n", aUserTime.uid, aUserTime.time) < 0) {
		err(11, "Error while printing user info");
	}
}

void print(const double dispersion, struct userTime* times, const uint32_t count) {

	uint32_t timePow2;
	for(uint32_t i = 0; i < count; i++) {
		timePow2 = times[i].time;
		timePow2 *= timePow2;

		if(dispersion < timePow2) {
			writeUserTime(times[i]);
		}
	}
}

int main(int argc, char** argv) {

	if(argc != 2) {
		errx(1, "Invalid number of parameters");
	}

	int fd = openFile(argv[1]);
	uint32_t size = getFileSize(fd, argv[1]);

	if(size % sizeof(struct entry) != 0) {
		errx(4, "Invalid data in file %s", argv[1]);
	}

	uint32_t countEntries = size / sizeof(struct entry);

	if(countEntries > MAX_ENTRIES) {
		errx(5, "Error too many entries in file %s", argv[1]);
	}

	dprintf(1, "Entries: %d\n", countEntries);
	struct entry entries[MAX_ENTRIES];
	struct userTime times[MAX_UNIQUE_ENTRIES];

	loadEntries(fd, argv[1], entries);

	double averageTime = getAverage(entries, countEntries);
	dprintf(1, "Average time: %f\n", averageTime);

	double dispersion = getDispersion(entries, countEntries, averageTime);
	dprintf(1, "Dispersion: %f\n", dispersion);


	uint32_t countUnique = loadMaxPerUID(entries, countEntries, times);

	print(dispersion, times, countUnique);


	close(fd);

	exit(0);
}
