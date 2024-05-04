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
#include <string.h>


int getNumFromStr(const char* str, const int pos);
void getFlagValue(const char* str, int* values);

void writeSymb(const char symb);

void flagCLogic(const int start, const int end);
void flagDLogic(const char del, const int start, const int end);

int getNumFromStr(const char* str, const int pos) {
	char symb = str[pos];

	if(symb < '1' || symb > '9') {
		errx(3, "Invalid flag value %s", str);
	}

	return symb - '0';
}

void getFlagValue(const char* str, int* values) {
	int length = strlen(str);

	if(length != 1 && length != 3) {
		errx(3, "Invalid flag value %s", str);
	}

	if(length == 1) {
		values[0] = getNumFromStr(str, 0);
		values[1] = values[0];
	}

	else if(length == 3) {
		if(str[1] != '-') {
			errx(3, "Invalid flag value %s", str);
		}

		values[0] = getNumFromStr(str, 0);
		values[1] = getNumFromStr(str, 2);
	}

	if(values[0] > values[1]) {
		errx(3, "Invalid flag value %s. %d is bigger than %d", str, values[0], values[1]);
	}
}

void writeSymb(const char symb) {
	if(write(1, &symb, sizeof(char)) < 0) {
		err(4, "Error while writing to stdout");
	}
}

void flagCLogic(const int start, const int end) {
	int read_count = 0;
	char symb;

	int i = 1;
	while((read_count = read(0, &symb, sizeof(char))) > 0) {

		if(symb == '\n') {
			writeSymb(symb);
			i = 0;
		}

		else if(i >= start && i <= end) {
			writeSymb(symb);
		}

		i++;
	}

	if(read_count < 0) {
		err(4, "Error while reading from stdin");
	}
}

void flagDLogic(const char del, const int start, const int end) {
	int read_count = 0;
	char symb;

	int i = 1;
	while((read_count = read(0, &symb, sizeof(char))) > 0) {

		if(symb == '\n') {
			writeSymb(symb);
			i = 1;
		}

		else if(i >= start && i <= end) {
			writeSymb(symb);
		}

		if(symb == del) {
			i++;
		}
	}

	if(read_count < 0) {
		err(4, "Error while reading from stdin");
	}

}

int main(int argc, char** argv) {

	if(argc != 3 && argc != 5) {
		errx(1, "Invalid number of parameters");
	}

	int values[2];
	char del;

	// cut -c 2-6
	if(argc == 3) {
		if(strcmp(argv[1], "-c") != 0) {
			errx(2, "Invalid flag option %s", argv[1]);
		}
		getFlagValue(argv[2], values);
		
		flagCLogic(values[0], values[1]);
	}

	// cut -d ':' -f 1-3
	else if(argc == 5) {
		if(strcmp(argv[1], "-d") != 0) {
			errx(2, "Invalid flag option %s", argv[1]);
		}

		if(strcmp(argv[3], "-f") != 0) {
			errx(2, "Invalid flag option %s", argv[3]);
		}

		del = argv[2][0];

		getFlagValue(argv[4], values);

		flagDLogic(del, values[0], values[1]);
	}

	exit(0);
}
