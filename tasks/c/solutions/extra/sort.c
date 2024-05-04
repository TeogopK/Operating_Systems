#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

// Sorts binary file, consisting of uint16_t
// sort(1)

void readNumbers(int fdIn, int fdOut);

void readNumbers(int fdIn, int fdOut)
{
	int bytes_count;
	uint16_t num;

	while( (bytes_count = read(fdIn, &num, sizeof(num))) > 0)
	{
		dprintf(fdOut, "%d\n", num);
	}

	if(bytes_count < 0) {
		err(6, "Error while reading from file");
	}
}

int main (int argc, char** argv)
{
	if(argc != 3) {
		warnx("usage: %s <input file> <output file>", argv[0]);
		errx(1, "%s sorts a file of unsigned 16-but integers", argv[0]);
	}

	int fdIn = open(argv[1], O_RDONLY);
	if(fdIn < 0) {
		err(2, "Error, could not open file %s", argv[1]);
	}

	int fdOut = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0666);
	if(fdOut < 0) {
		err(2, "Error, could not open file %s", argv[2]);
	}

	struct stat fileStat;
	if(fstat(fdIn, &fileStat) < 0) {
		err(3, "Error could not get the stat of file %s", argv[1]);
	}

	if(fileStat.st_size % (sizeof(uint16_t)) != 0) {
		errx(4, "Error file %s size is incorrect", argv[1]);
	}

	pid_t pid;

	int pipeFirst[2];
	if(pipe(pipeFirst) < 0) {
		err(5, "Error while creating pipe");
	}

	pid = fork();

	if(pid < 0) {
		err(6, "Error while forking");
	}

	if(pid == 0) {
		close(pipeFirst[0]);

		readNumbers(fdIn, pipeFirst[1]);

		close(pipeFirst[1]);
		exit(0);
	} else {
		close(pipeFirst[1]);

		int status;
		if(wait(&status) < 0) {
			err(20, "Waiting child failed");
		}

		if(!WIFEXITED(status)) {
			errx(21, "Child did not exited");
		}
	}

	// second child

	int pipeSecond[2];
	if(pipe(pipeSecond) < 0) {
		err(5, "Error while creating second pipe");
	}

	pid = fork();

	if(pid < 0) {
		err(6, "Error while forking second time");
	}

	if(pid == 0) {

		close(pipeSecond[0]);

		if(dup2(pipeFirst[0], 0) < 0) {
			err(10, "Stdin duplication failed");
		}

		if(dup2(pipeSecond[1], 1) < 0) {
			err(11, "Stout duplication failed");
		}

		if(execlp("sort", "sort", "-n", 0) < 0) {
			err(12, "Error, sort failed");
		}

	} else {
		close(pipeSecond[1]);

		int status;
		if(wait(&status) < 0) {
			err(20, "Waiting child failed");
		}

		if(!WIFEXITED(status)) {
			errx(21, "Child did not exited");
		}

		uint16_t n;
		int32_t read_count = 0;

		while((read_count = read(pipeSecond[0], &n, sizeof(uint16_t))) > 0) {
			if(write(fdOut, &n, sizeof(uint16_t)) != sizeof(uint16_t)) {
				err(23, "Error while writing to output file");
			}
		}

		if(read_count < 0) {
			err(22, "Error while reading from second pipe");
		}

	}

	close(fdIn);
	close(fdOut);

	exit(0);
}


