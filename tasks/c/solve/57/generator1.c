#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

int main(void) {

	int fd = open("f1", O_CREAT | O_TRUNC| O_WRONLY, 0666);
	uint32_t arr[6];

	arr[0] = 1;
	arr[1] = 2;
	arr[2] = 9;
	arr[3] = 3;
	arr[4] = 16;
	arr[5] = 4;

	write(fd, arr, sizeof(uint32_t) * 6); 
}
