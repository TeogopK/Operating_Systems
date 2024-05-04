#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

int main(void) {

	int fd = open("f2", O_CREAT | O_TRUNC| O_WRONLY, 0666);
	uint32_t arr[20];

	arr[0] = 0;
	for(int i = 1; i < 20; i++) {
		arr[i]+= arr[i - 1] + 1;
	}

	write(fd, arr, sizeof(uint32_t) * 20); 
}
