#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <err.h>

void safePrint(const int fd, const char* text);
void safePrintParameter(const int fd, const char* text, const uint32_t num);

void safePrint(const int fd, const char* text) {

	if(dprintf(fd, text) < 0) {
		err(6, "Error while printing text");
	}
}

void safePrintParameter(const int fd, const char* text, const uint32_t num) {
	if(dprintf(fd, text, num) < 0) {
		err(6, "Error while printing text");
	}
}

int main(int argc, char** argv) {

	if( argc != 3) {
		errx(1, "Invalid number of parameters");
	}

	int fdin = open(argv[1], O_RDONLY);

	if(fdin < 0) {
		err(2, "Error while opening file %s", argv[1]);
	}

	struct stat st;

	if(fstat(fdin, &st) < 0) {
		err(3, "Error while fstating file %s", argv[1]);
	}

	uint32_t size = st.st_size;

	if(size % sizeof(uint16_t) != 0) {
		errx(4, "Invalid data in file %s", argv[1]);
	}

	uint32_t count = size / sizeof(uint16_t);

	if(count > 524288) {
		errx(4, "Invalid data in file %s", argv[1]);
	}

	const char* LINE1 = "#include <stdint.h>\n";
	const char* LINE2 = "const uint16_t arr [] = {\n";
	const char* LINE3 = "};\n";
	const char* LINE4 = "const uint32_t arrN = %d;\n";

	const char* LINE_NUMBER = "	0x%04x,\n";
	
	int fdout = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0666);

	if(fdout < 0) {
		err(5, "Error while opening file %s", argv[2]);
	}

	safePrint(fdout, LINE1);
	safePrint(fdout, LINE2);

	int32_t read_count = 0;
	uint16_t num;

	while( (read_count = read(fdin, &num, sizeof(uint16_t))) == sizeof(uint16_t)) {
		safePrintParameter(fdout, LINE_NUMBER, (uint32_t)num);
	}

	if(read_count != 0) {
		err(7, "Error while reading data from file %s", argv[1]);
	}


	safePrint(fdout, LINE3);
	safePrintParameter(fdout, LINE4, count);


	close(fdin);
	close(fdout);

	exit(0);
}
