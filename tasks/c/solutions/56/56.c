#include <stdio.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>


int main(int argc, char** argv)
{

	if(argc != 2)
	{
		errx(1, "Invalid number of parameters");
	}

	int fd = open(argv[1], O_RDWR);

	if(fd < 0)
	{
		err(2, "Error while opening file %s", argv[1]);
	}

	uint8_t arr[256];

	for(uint16_t i = 0 ; i < 256; i++)
	{
		arr[i] = 0;
	}

	int bytes_count = 0;
	uint8_t sym;
	while( (bytes_count = read(fd, &sym, sizeof(sym))) > 0)
	{
		arr[sym]++;
	}

	if(bytes_count > 0)
	{
		err(3, "Error while reading from file %s", argv[1]);
	}

	if(lseek(fd, SEEK_SET, 0) < 0)
	{
		err(4, "Error while lseeking in file %s", argv[1]);
	}

	//	close(fd);
	//	open(argv[1], O_WRONLY | O_TRUNC);

	uint8_t i = 0;
	do
	{
		for(int j = 0; j < arr[i]; j++)
		{
			if(write(fd, &i, sizeof(sym)) < 0)
			{
				err(5, "Error while writing in file %s", argv[1]);
			}
		}
	}
	while(i++ != 255);

	if(close(fd) < 0)
	{
		err(6, "Error while closing file");
	}
}
