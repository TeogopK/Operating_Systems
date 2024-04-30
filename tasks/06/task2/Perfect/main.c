/*
	Реализирайте команда head без опции (т.е. винаги да извежда
	на стандартния изход само първите 10 реда от съдържанието на
	файл подаден като първи параматър)
*/

/*
 * Upgraded version
 *
 * Usage:
 * 	head --> reads from stdin the default number of rows
 * 	head file1 --> reads from file1 the default number of rows
 * 	head -n <number> --> reads the specified number of rows from stdin
 * 	head -n <number> file1 --> reads the specified number of rows from file1
 *
 * 	head -n --> INVALID
 * 	head file1 file2 --> INVALID
 * 	head -n <not a number> --> INVALID
 * 	head -n <number> file1 file2 --> INVALID
 * 	head <incorrect option> <sth> ... ---> INVALID
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>
#include <fcntl.h>
#include <string.h>

int openFile(char * path);
void closeFile(int fd, char * path);
void writeFromFD(int fd, uint16_t maxRows);
void writeFromFile(char* path, uint16_t maxRows);
uint16_t getMaxRows(char* str);

const uint8_t SEPARATOR = '\n';

const uint16_t MAX_ROWS_DEFAULT_VALUE = 10;
const int MAX_ROWS_OPTION_POSITION = 2;
const char* MAX_ROWS_OPTION_KEY = "-n";

int main(int argc, char ** argv)
{
	uint16_t maxRows = MAX_ROWS_DEFAULT_VALUE;
	
	if(argc == 1)
	{
		writeFromFD(0, maxRows);
	}
	
	else if(argc == 2)
	{
		if(strcmp(argv[1], MAX_ROWS_OPTION_KEY) == 0)
		{
			errx(101, "Error: No value given for %s option", MAX_ROWS_OPTION_KEY);
		}

		writeFromFile(argv[1], maxRows);
	}

	else if(argc >= 3)
	{
		if(strcmp(argv[1], MAX_ROWS_OPTION_KEY) != 0)
		{
			errx(102, "Error: Expected an option if parameters are more than 1 file");
		}


		maxRows = getMaxRows(argv[2]);

		if(argc == 3)
		{
			writeFromFD(0, maxRows);
		}

		else if(argc == 4)
		{
			writeFromFile(argv[3], maxRows);
		}

		else
		{
			errx(103, "Error: Too many arguments");
		}
	}

	exit(0);
}

void writeFromFile(char* path, uint16_t maxRows)
{
	int fd = openFile(path);

	writeFromFD(fd, maxRows);

	closeFile(fd, path);
}

void writeFromFD(int fd, uint16_t maxRows)
{
	if(maxRows <= 0)
	{
		return;
	}

	uint8_t sym;
	int16_t read_count = 0;

	uint16_t countRows = 0;

	ssize_t
	while( (read_count = read(fd, &sym, sizeof(sym))) > 0 )
		
	{
		if(sym == SEPARATOR)
		{
			countRows++;
		}

		if(write(1, &sym, sizeof(sym)) == -1)
		{
			err(4, "Error: Writing on stdout");
		}	

		if(countRows >= maxRows)
		{
			break;
		}

	}

	if(read_count == -1)
	{
		err(3, "Error: Reading from the given source");
	}

}

int openFile(char * path)
{
	int fd = open(path, O_RDONLY);
	if(fd == -1)
	{
		err(2, "Error: Opening file %s", path);
	}

	return fd;
}

void closeFile(int fd, char * path)
{
	if(close(fd) == -1)
	{
		err(9, "Error: Closing file %s", path);
	}
}


uint16_t getMaxRows(char* str)
{
	uint16_t maxRows = 0;

	for(int i = 0; str[i] != '\0'; i++)
	{
		if(str[i] < '0' || str[i] > '9')
		{
			errx(104, "Error: Value is not a positive integer");
		}
		maxRows *= 10;
		maxRows += str[i] - '0';
	}

	return maxRows;
}
