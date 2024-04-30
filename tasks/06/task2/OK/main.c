/*
	Реализирайте команда head без опции (т.е. винаги да извежда
	на стандартния изход само първите 10 реда от съдържанието на
	файл подаден като първи параматър)
*/



#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>
#include <fcntl.h>


int openFile(char * path);
void closeFile(int fd, char * path);

int openFile(char * path)
{
	int fd = open(path, O_RDONLY);
	if(fd == -1)
	{
		err(2, "Error: Opening file %s", path);
	}

	return fd;
}

int main(int argc, char ** argv)
{
	if(argc != 2)
	{
		errx(1, "Error: Invalid number of parameters");
	}

	int fd = openFile(argv[1]);

	uint8_t sym;
	int16_t read_count = 0;

	const int MAX_ROWS = 10;
	const uint8_t SEPARATOR = '\n';

	uint16_t countRows = 0;

	while( (read_count = read(fd, &sym, sizeof(sym))) > 0 )
	{
		if(sym == SEPARATOR)
		{
			countRows++;
		}

		if(write(0, &sym, sizeof(sym)) == -1)
		{
			err(4, "Error: Writing on stdout");
		}	

		if(countRows >= MAX_ROWS)
		{
			break;
		}

	}

	if(read_count == -1)
	{
		err(3, "Error: Reading from file %s", argv[1]);
	}

	closeFile(fd, argv[1]);

	exit(0);
}


void closeFile(int fd, char * path)
{
	if(close(fd) == -1)
	{
		err(9, "Error: Closing file %s", path);
	}
}
