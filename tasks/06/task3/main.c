//  Реализирайте команда wc, с един аргумент подаден като входен параметър.
#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int openFile(char* path);
void printNumber(int fd, const char* text, int num);
void countTextOfFile(int fd);
void closeFile(int fd);

int openFile(char* path)
{
	int fd;
	fd = open(path, O_RDONLY);

	if(fd < 0)
	{
		err(2, "Error while opening file %s", path);
	}

	return fd;
}

void closeFile(int fd)
{
	if(close(fd) < 0)
	{
		err(5, "Error while closing file with fd %d", fd);
	}
}

void printNumber(int fd, const char* text, int num)
{
	if(dprintf(fd, text, num) < 0)
	{
		err(4, "Error while writing to fd %d", fd);
	}
}

void countTextOfFile(int fd)
{
	int words = 0;
	int chars = 0;
	int lines = 0;

	char symb;

	int count_bytes = 0;

	while( (count_bytes = read(fd, &symb, sizeof(symb))) > 0)
	{
		if(symb == '\n')
		{
			lines++;
			words++;
		}
		else if(symb == ' ')
		{
			words++;
		}
		chars++;
	}

	if(lines == 0)
	{
		lines = 1;
	}

	if(symb != '\n')
	{
		words++;
	}

	if(count_bytes < 0)
	{
		err(3, "Error while reading from file with fd %d", fd);
	}

	printNumber(0, "Count of characters: %d\n", chars);
	printNumber(0, "Count of words: %d\n", words);
	printNumber(0, "Count of lines: %d\n", lines);

	closeFile(fd);
}

int main(int argc, char** argv)
{
	int fd;

	if(argc == 1)
	{
		fd = 0;
	}
	else if(argc == 2)
	{
		fd = openFile(argv[1]);
	}
	else
	{
		errx(1, "Too many arguments given");
	}

	countTextOfFile(fd);

	exit(0);
}
