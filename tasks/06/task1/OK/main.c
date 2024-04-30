// копирайте съдържанието на файл1 във файл2

#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdlib.h>
#include <stdint.h>

int openFileReadOnly(char * path);
int openFileWrite(char * path);
void closeFile(int fd);

int openFileReadOnly(char * path)
{
	int fd = open(path, O_RDONLY);
	if(fd == -1)
	{
		err(2, "Error: Opening file %s", path);
	}

	return fd;
}

int openFileWrite(char * path)
{
	int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(fd == -1)
	{
		err(2, "Error: Opening file %s", path);
	}

	return fd;
}

int main (int args, char ** argv)
{
	if(args != 3)
	{
		errx(1, "Error: Wrong number of parameters");
	}

	int fdSource, fdDestination;

	fdSource = openFileReadOnly(argv[1]);
	fdDestination = openFileWrite(argv[2]);
	
	uint8_t sym;
	int16_t bytes_count = 0;

	while( (bytes_count = read(fdSource, &sym, sizeof(sym)) ) > 0)
	{
		if(write(fdDestination, &sym, sizeof(sym)) == -1)
		{
			err(4, "Error: while writing in file %s", argv[2]);
		}
	}

	if(bytes_count == -1)
	{
		err(3, "Error: reading from file %s", argv[1]);
	}



	closeFile(fdSource);
	closeFile(fdDestination);

	exit(0);
}

void closeFile(int fd)
{
	int exitStatus = close(fd);
	if(exitStatus < 0)
	{
		err(9, "Error: Closing file");
	}
}
