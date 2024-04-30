#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct triplet{
	char name [8];
	uint32_t offset;
	uint32_t length;
};

int main(int argc, char** argv) {

	if(argc != 2) { 
		warnx("Usage is <%s> <%s>", argv[1], argv[2]);
		errx(1, "Invalid number of parameters");
	}

	int fd;
	if( (fd = open(argv[1], O_RDONLY)) < 0) {
		err(2, "Error, failed to open file %s", argv[1]);
	}

	struct stat fileStat;

	if(fstat(fd, &fileStat) < 0) {
		err(3, "Error, failed to stat file %s", argv[1]);
	}

	if((fileStat.st_size % sizeof(struct triplet)) != 0) {
		errx(4, "Error, file %s size is of incorrect size", argv[1]);
	}

	int count = fileStat.st_size / sizeof(struct triplet);

	int p[2];
	if(pipe(p) < 0) {
		err(5, "Failed to open a pipe");
	}

	struct triplet tripletsArr [8];
	int bytes_count = 0;

	if( (bytes_count = read(fd, tripletsArr,  sizeof(struct triplet) * count)) < 0) {
		err(6, "Failed to read from source file");
	}

	pid_t pid;

	for(int i = 0; i < count; i++) {
		pid = fork();

		if(pid < 0) {
			err(6, "Failed to fork a child %i", i);
		}

		if(pid == 0) {
			uint16_t acummulator = 0x0000;

			int data_fd;
			if( (data_fd = open(aTriplet.name, O_RDONLY)) < 0) {
				err(2, "Error, failed to open file %s", aTriplet.name);
			}

			if(lseek(data_fd, aTriplet.offset * 2, ) < 0) {
				err(8, "Failed to lseek in file %s to position %ld", aTriplet.name, aTriplet.offset);
			}
		}
		else {

		}




	}


	exit(0);
}

