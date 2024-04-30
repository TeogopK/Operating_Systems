#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

const int MAX_FILES = 100;
const int MAX_FILE_NAME = 255;
const int HASH_SIZE = 32;

#define filesType char files[MAX_FILES][MAX_FILE_NAME]

void pipeSafe(int *p);
void waitChild(const char* childname);

int getFiles(filesType, const char* dir);
uint32_t readFileNames(const int pipefd, filesType);

int forkChilds(const int count);
void childLogic(const char* file);
void saveHash(const char* file, const char* hash);

void pipeSafe(int *p) {
	if(pipe(p) < 0) {
		err(2, "Error while creating pipe");
	}
}

int getFiles(filesType, const char* dir) {
	int files_count = 0;

	int p[2];
	pipeSafe(p);

	pid_t pid = fork();

	if(pid < 0) {
		err(3, "Error while forking child find");
	}

	if(pid == 0) {
		close(p[0]);

		if(dup2(p[1], STDOUT_FILENO) < 0) {
			err(20, "Error while redirecting output in child find");
		}

		if(execlp("find", "find", dir, "-type", "f", "-not", "-name", "*.hash", (char *) NULL) < 0) {
			err(21, "Error while exec find in child");
		}

	} else {
		close(p[1]);

		waitChild("child executing find");

		files_count = readFileNames(p[0], files);

		close(p[0]);
	}

	return files_count;
}

void waitChild(const char* childname) {

	int status;

	if(wait(&status) < 0) {
		err(10, "Error while waiting %s", childname);
	}

	if(!WIFEXITED(status)) {
		errx(11, "Error: %s did not exit normally", childname);
	}

	if(WEXITSTATUS(status) != 0) {
		errx(12, "Error: %s did not exit with exit code 0", childname);
	}
}

uint32_t readFileNames(const int pipefd, filesType) {
	uint32_t files_count = 0;

	uint8_t symb;
	int32_t read_count = 0;
	int i = 0;

	while( (read_count = read(pipefd, &symb, sizeof(symb))) > 0) {
		if(i >= MAX_FILE_NAME) {
			errx(13, "File is too long to be saved");
		}

		if(files_count >= MAX_FILES) {
			errx(14, "Too many files to be loaded");
		}

		files[files_count][i] = symb;
		i++;

		if(symb == '\n') {
			files[files_count][i - 1] = '\0';
			files_count++;

			i = 0;
		}
	}

	return files_count;
}

int forkChilds(const int count) {

	for(int i = 0; i < count; i++) {
		pid_t pid = fork();

		if(pid < 0) {
			err(4, "Error while forking md5sum children");
		}

		if(pid == 0) {
			// each child process has id from 0 to files_count - 1
			return i;
		}
	}

	// process with id = files_count will be the father of them all
	return count;
}

void childLogic(const char* file) {

	int p[2];
	pipeSafe(p);

	pid_t pid = fork();

	if(pid < 0) {
		err(30, "Error while forking third layer child");
	}

	if(pid == 0) {

		close(p[0]);

		if(dup2(p[1], STDOUT_FILENO) < 0) {
			err(31, "Error while redirecting output in md5sum child");
		}

		if(execlp("md5sum", "md5sum", file, (char *) NULL) < 0){
			err(32, "Error while getting the md5sum of file %s in child", file);
		}

	} else {
		close(p[1]);

		waitChild("grandchild");

		char md5sum [HASH_SIZE + 1];

		if(read(p[0], md5sum, HASH_SIZE) != HASH_SIZE) {
			err(33, "Error while reading md5sum hashsum");
		}
		md5sum[HASH_SIZE] = '\0';

		saveHash(file, md5sum);

		close(p[0]);
		exit(0);
	}

}

void saveHash(const char* file, const char* md5sum) {

	char hashfile [MAX_FILE_NAME + 6];

	strcat(hashfile, file);
	strcat(hashfile, ".hash\0");

	int fd = open(hashfile, O_WRONLY | O_TRUNC | O_CREAT, 0666);

	if(fd < 0) {
		err(34, "Error while opening hashfile %s", hashfile);
	}

	if(write(fd, md5sum, HASH_SIZE + 1) != HASH_SIZE + 1) {
		err(35, "Error while writing md5sum in file");
	}

	dprintf(STDOUT_FILENO, "%s created with hashsum %s\n", hashfile, md5sum);
}

int main(int argc, char** argv) {

	if(argc != 2) {
		err(1, "Program usage is %s <Dir> to create hash files with md5sum", argv[0]);
	}

	char files[MAX_FILES][MAX_FILE_NAME];

	int files_count = getFiles(files, argv[1]);

	int id = forkChilds(files_count);

	if(id < files_count) {
		childLogic(files[id]);

	} else {
		while(wait(NULL) > 0);
	}

	//only father will be able to execute this code
	dprintf(STDOUT_FILENO, "I have waited them all %d! Now I can finally rest!\n", files_count);

	exit(0);
}

