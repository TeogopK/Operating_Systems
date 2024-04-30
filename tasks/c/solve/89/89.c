#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <err.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

const char L [3][5] = {"tic ", "tac ", "toe\n"};

const int MAX_WC = 35;
const int MAX_NC = 7;

void printWord(const char* word);
void sendCount(const int fd, const int count);

void printWord(const char* word) {
	if(write(STDOUT_FILENO, word, strlen(word)) < 0) {
		err(6, "Error while writing to stdout");
	}
}

void sendCount(const int fd, const int count) {
	if(write(fd, &count, sizeof(int)) < 0) {
		err(7, "Error while writing to pipe with fd %d", fd);
	}
}

int main(int argc, char** argv) {

	if(argc != 3) {
		errx(1, "Program usage is %s <Number of processes> <Number of words>", argv[0]);
	}

	//GETING ARGUMENTS
	
	int NC = atoi(argv[1]);

	if(NC < 1 || NC > MAX_NC) {
		errx(2, "Invalid number of processes value %d", NC);
	}

	int WC = atoi(argv[2]);
	if(WC < 1 || WC > MAX_WC) {
		errx(3, "Invalid count of total words value %d", WC);
	}

	// CREATING PIPES
	int pipes[MAX_NC][2];

	for(int i = 0; i < NC + 1; i++) {
		if(pipe(pipes[i]) < 0) {
			err(4, "Error while creating pipes");
		}
	}

	// CREATING PROCESSES WITH IDS
	//
	// id = NC is for parent
	int id = NC;

	for(int i = 0; i < NC; i++) {
		pid_t pid = fork();

		if(pid < 0) {
			err(5, "Error while forking");
		}
		
		if(pid == 0) {
			id = i;
			break;
		}
	}

	// CLOSING PIPES NOT USED
	if(id == NC) {
		//parent	
		close(pipes[0][0]);
		close(pipes[NC][1]);

		for(int i = 1; i < NC; i++) {
			close(pipes[i][0]);
			close(pipes[i][1]);
		}
	}
	else {
		for(int i = 0; i < NC + 1; i++) {
			if(id == i) {
				close(pipes[i][1]);
				close(pipes[i + 1][0]);
				i++;
			} else {
				close(pipes[i][0]);
				close(pipes[i][1]);
			}

		}
	}

	int count = 0;

	// PARENT STARTING	
	if(id == NC) {
		printWord(L[count]);
		count++;
		sendCount(pipes[0][1], count);
	}

	// SYNCHRONOUS READING WRITING
	while(read(pipes[id][0], &count, sizeof(int))) {
		if(count >= WC) {
			break;
		}

		// PRINTING WORD
		printWord(L[count % 3]);
		count++;

		// SENDING INFO
		if(id == NC) {
			sendCount(pipes[0][1], count);
		} else {
			sendCount(pipes[id + 1][1], count);
		}
	}


	exit(0);
}
