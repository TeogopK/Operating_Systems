#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <err.h>

int main(int argc, char ** argv)
{
	if( argc != 2 ){
		//errx when there is an user error (business logic error)
		errx(1, "ERROR: Arg count");
	}


	int bytes[256];

	for(int i = 0; i < 256; i++)
	{
		bytes[i] = 0;
	}

	char* file_name = argv[1];

	// also ok

	// int fd;
	// if(( fd = open(argv[1], O_RDWR) == -1 ))
	// {
	// 	err(2, "ERROR: Opening file %s", argv[1]);
	// }

	int fd = open(file_name, O_RDWR);
	if(fd == -1)
	{
		//system error so we use 'err' withoout x
		//also reads and pritns the system error
		err(2, "ERROR: Opening file %s", argv[1]);
	}

	uint8_t buf; //unsigned int 8 bits

	// THERE IS NO CHECK!

	// can be 1 -> byte, instead of sizeof()	
	// while(read(fd, &buf, sizeof(buf)) > 0) {
	// 	bytes[buf] += 1;
	// }

	int8_t bytes_count;
	while(( bytes_count = read(fd, &buf, sizeof(buf))) > 0 ) {
		bytes[buf] += 1;
	}

	if(bytes_count < 0){
		err(3, "ERRROR: Reading from file");
	}

	lseek(fd, 0, SEEK_SET);

	for(uint16_t i = 0; i < 256; i++) {
		// for(uint8_t i = 0; i < 256; i++) { // cycle is never ending
		uint8_t temp = i;

		for(int j = 0 ; j < bytes[i]; j++) {
			if(write(fd, &temp, sizeof(temp)) < 0){
				err(4, "ERRROR: Writing from file");
			}

		}
	}

	close(fd); // wanted by the government

	// exit(0);
	return 1;
	}
