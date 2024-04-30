#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>

int main(int argc, char** argv) {

	sleep(argc);

	int exit = atoi(argv[argc - 1]);
	return exit;
}
