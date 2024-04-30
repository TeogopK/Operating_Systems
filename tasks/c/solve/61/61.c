#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <err.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int openFile(const char* file);
int readFile(const int fd, const char* file, char* buff, const int countFlag, const int linesCount);
void writeToStdout(const char* buff, const int count);
void writeCountToStdout(const int count);

const char* COUNT_STRING = "-n";
const char* STDIN_STRING = "-";
const int MAX_BUFF_SIZE = 1024;

int readFile(const int fd, const char* file, char* buff, const int countFlag, const int linesCount) {
	int newCount = linesCount;
	int read_count = 0;

	char symb;
	int i = 0;
	while( (read_count = read(fd, &symb, sizeof(char))) > 0) {

		buff[i] = symb;
		i++;

		if(symb == '\n') {

			if(countFlag == 1) {
				writeCountToStdout(newCount);
				newCount++;
			}

			writeToStdout(buff, i);

			i = 0;
		}
	}

	if(read_count < 0) {
		err(2, "Error while reading from file %s", file);
	}

	return newCount;
}

void writeToStdout(const char* buff, const int count) {
	//strlen(buff) does not work
	if(write(1, buff, count) < 0) {
		err(3, "Error while writing to STDOUT");
	}
}

void writeCountToStdout(const int count) {
	if(dprintf(1, "%d ", count) < 0) {
		err(3, "Error while writing count to STDOUT");
	}
}

int openFile(const char* file) {
	int fd = open(file, O_RDONLY);

	if(fd < 0) {
		err(4, "Error while opening file %s", file);
	}

	return fd;
}

int main (int argc, char** argv) {

	char buff[MAX_BUFF_SIZE];

	int countFlag = 0;
	int startFile = 1;

	int linesCount = 1;

	if(argc <= 1) {
		readFile(0, "STDIN", buff, countFlag, 0);
		exit(0);
	}
	else if(strcmp(argv[1], COUNT_STRING) == 0) {
		countFlag = 1;
		startFile = 2;
	}

	if(argc == 2 && countFlag == 1) {
		readFile(0, "STDIN", buff, countFlag, linesCount);
	}

	for(int i = startFile; i < argc; i++) {

		if(strcmp(argv[i], STDIN_STRING) == 0) {
			linesCount = readFile(0, "STDIN", buff, countFlag, linesCount);
		} else {
			int fd = openFile(argv[i]);

			linesCount = readFile(fd, argv[i], buff, countFlag, linesCount);

			close(fd);
		}
	}

	exit(0);
}
