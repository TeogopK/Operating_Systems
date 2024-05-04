#include <err.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>

void showPrompt(void);
void readInput(char* input);
void transferInput(char* input, char** commandArg);
void executeCommand(char** commandArg);
void forkCommand(char *input, char** commandArg);

const char* EXIT = "exit";
const int MAX_BUF = 1024;
const int MAX_ARG = 100;

void showPrompt(void) 
{
	const char* prompt = "> ";
	if(write(0, prompt, strlen(prompt)) < 0) {
		err(2, "Error while showing prompt");
	}
}

void readInput(char* input)
{
	int bytes_count = 0;
	char symb = 'a';

	int i = 0;
	while(i < MAX_BUF - 1)
	{
		bytes_count = read(0, &symb, sizeof(symb));
		if(bytes_count < 0) {
			err(3, "Error while reading input");
		}

		input[i] = symb;

		if(symb == '\n') {
			break;
		}

		i++;
	}

	input[i] = '\0';
}

void transferInput(char* input, char** commandArg)
{
	int argPosition = 0;
	int startArg = 0;

	for(int i = 0; input[i] != '\0'; i++)
	{
		if(input[i] == ' ') {

			if(argPosition >= MAX_ARG - 2){
				break;
			}

			input[i] = '\0';

			commandArg[argPosition] = input + startArg;
			startArg = i + 1;
			argPosition++;
		}
	}

	commandArg[argPosition] = input + startArg;
	commandArg[argPosition + 1] = NULL;
}

void executeCommand(char** commandArg)
{
	if(execvp(commandArg[0], commandArg) < 0) {
		err(4, "Error while executing command %s", commandArg[0]);
	}
}

void forkCommand(char *input, char** commandArg)
{
	readInput(input);
	transferInput(input, commandArg);

	if(strcmp(input, EXIT) == 0) {
		exit(0);
	}

	pid_t pid = fork();

	if(pid < 0) {
		err(5, "Error while forking process");
	}

	if(pid == 0) {
		executeCommand(commandArg);
	} else {
		int signal;

		if(wait(&signal) < 0) {
			err(6, "Error, failed to wait child");
		}

		if(!WIFEXITED(signal)) {
			warnx("Error, child failed to exit normally");
		}

		if(WEXITSTATUS(signal) != 0) {
			warnx("Error, child failed to exit with status 0");
		}
	}
}

int main(int argc, char** argv)
{
	if(argc != 1) {
		errx(1, "Program %s does not need any parameters", argv[0]);
	}

	char input [MAX_BUF];
	char* commandArg [MAX_ARG];

	while(true)
	{
		showPrompt();
		forkCommand(input, commandArg);
	}

}

