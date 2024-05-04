#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <strings.h>
#include <string.h>

const char* DELETE_STRING = "-d";
const char* SQUEEZE_STRING = "-s";

void safeWriteToStdout(const char symb);
bool checkIsInGroup(const char symb, const char* group);
int getIndex(const char symb, const char* arr);

void deleteLogic(char** argv);
void squeezeLogic(char** argv);
void replaceLogic(char** argv);


bool checkIsInGroup(const char symb, const char* group) {
	return strchr(group, symb) != NULL;
}

void deleteLogic(char** argv) {

	int read_bytes = 0;
	char symb;

	while( (read_bytes = read(0, &symb, sizeof(char))) > 0) {
		if(checkIsInGroup(symb, argv[2])) {
			continue;
		}
		safeWriteToStdout(symb);	
	}

	if(read_bytes < 0) {
		err(2, "Error while reading in delete function");
	}
}

void squeezeLogic(char** argv) {

	int read_bytes = 0;
	char symb;
	char prev;

	bool firstTime = true;
	while( (read_bytes = read(0, &symb, sizeof(char))) > 0) {
		if(firstTime) {
			firstTime = false;
		}

		else if(checkIsInGroup(symb, argv[2]) && symb == prev) {
			continue;
		}

		safeWriteToStdout(symb);	
		prev = symb;
	}

	if(read_bytes < 0) {
		err(2, "Error while reading in squeeze function");
	}
}

void replaceLogic(char** argv) {

	if(strlen(argv[1]) != strlen(argv[2])) {
		errx(10, "Invalid length of replace string");
	}

	int read_bytes = 0;
	char symb;

	char newSymb;
	int indexSymb;

	while( (read_bytes = read(0, &symb, sizeof(char))) > 0) {
		if(checkIsInGroup(symb, argv[1])) {

			indexSymb = getIndex(symb, argv[1]);
			newSymb = argv[2][indexSymb];

			safeWriteToStdout(newSymb);	
		}
		else {
			safeWriteToStdout(symb);	
		}
	}

	if(read_bytes < 0) {
		err(2, "Error while reading in replace function");
	}
}

int getIndex(const char symb, const char* arr) {

	for(int i = 0; arr[i] != '\0'; i++) {
		if(arr[i] == symb) {
			return i;
		}
	}
	return -1;
}

void safeWriteToStdout(const char symb) {
	if(write(1, &symb, sizeof(char)) < 0) {
		err(3, "Error while writing to stdout");
	}
}


int main (int argc, char** argv) {

	if(argc != 3) {
		warnx("Script usage is %s [OPTION] SET1 [SET2]", argv[0]);
		errx(1, "Invalid number of parameters");
	}

	if(strcmp(argv[1], DELETE_STRING) == 0) {
		deleteLogic(argv);
	}

	else if(strcmp(argv[1], SQUEEZE_STRING) == 0) {
		squeezeLogic(argv);
	}

	else{
		replaceLogic(argv);
	}

	exit(0);
}
