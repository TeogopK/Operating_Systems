#include <stdlib.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdbool.h>

const char* STDIN_FLAG = "-";

const int STDIN_FD = 0;
const char* STDIN_NAME = "stdin";

const int STDOUT_FD = 1;
const char* STDOUT_NAME = "stdout";


int openFile(const char* file);
void readFile(const int fd, const char* file);
void writeFile(const int fd, const char* file, const char symb);
bool stdinCheck(const char* file);

void readFile(const int fd, const char* file) {
	int read_count = 0;

	char symb;
	while( (read_count = read(fd, &symb, sizeof(char))) > 0) {

		writeFile(STDOUT_FD, STDOUT_NAME, symb);	
	}

	if(read_count < 0) {
		err(3, "Error while reading from file %s", file);
	}

}

void writeFile(const int fd, const char* file, const char symb) {

	if(write(fd, &symb, sizeof(char)) < 0) {
		err(4, "Error while writing to %s", file);
	}
}

int openFile(const char* file) {
	int fd = open(file, O_RDONLY);

	if(fd < 0) {
		err(2, "Error while opening file %s", file);
	}

	return fd;
}

bool stdinCheck(const char* file) {
	return strcmp(file, STDIN_FLAG) == 0;
}

int main(int argc, char** argv) {

	if(argc == 1) {
		readFile(STDIN_FD, STDIN_NAME);
		exit(0);
	}

	for(int i = 1; i < argc; i++) {

		if(stdinCheck(argv[i])) {

			readFile(STDIN_FD, STDIN_NAME);

		} else{

			int fd = openFile(argv[i]);
			readFile(fd, argv[i]);

			close(fd);
		}

	}

	exit(0);
}
