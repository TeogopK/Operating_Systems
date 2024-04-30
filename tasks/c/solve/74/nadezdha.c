#include <stdint.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

//#include "test-out.h"

int main(const int argc, const char* argv[])
{
	if (argc != 3) {
		errx(1, "%s in out", argv[0]);
	}

	int fd1 = open(argv[1], O_RDONLY);
	if (fd1 == -1) {
		err(2, "Cannot open %s", argv[1]);
	}

	struct stat st;
	if (fstat(fd1, &st) == -1) {
		err(3, "Cannot stat %s", argv[1]);
	}

	if ((st.st_size % sizeof(uint16_t)) != 0) {
		errx(2, "wrong in size");
	}

	off_t out_sz = st.st_size / sizeof(uint16_t);

	if (out_sz > 524288) {
		errx(3, "file too big");
	}

	if (out_sz < 1) {
		errx(4, "file empty");
	}


	int fd2 = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (fd2 == -1) {
		err(6, "Cannot open %s", argv[2]);
	}

	const char* LINE1 = "#include <stdint.h>\n";
	const char* LINE2 = "const uint16_t arr[] = {\n";
	const char* LINE3 = "};\n";
	const char* LINE4 = "const uint32_t arrN = %d;";

	//here we will not have problem with buffering because we are using only dprintf and not a combination of write
	dprintf(fd2, LINE1);
	dprintf(fd2, LINE2);

	uint16_t e = 0;
	int r = 0;


	while ((r = read(fd1, &e, sizeof(e))) == sizeof(e)) {
		dprintf(fd2, "0x%04x,\n", e);
	}

	if (r != 0) {
		err(7, "err read in");
	}

	dprintf(fd2, LINE3);
	dprintf(fd2, LINE4, out_sz);

	close(fd1);
	close(fd2);

	return 0;
}

