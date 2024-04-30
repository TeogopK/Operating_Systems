#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


// naredeni troiki --> size % 3
// otmestvaneto + daljinata > goleminata na saiza
// .dat .idx --> doplanitelna proverka

stuct pos{
	uint16_t pos;
	uint8_t length;
	uint8_t _;
}

int main ( int argc, char ** argv)
{
	if(argc != 5)
	{
		errx(1, "Invalid number of parameters")
	}

	int dat_fd, indx_fd;
	
	if(( dat_fd = open(argv[1], O_RDONLY) ) == -1){
		err(1, "ERROR: Opening file %s", argv[1]);
	}

	if(( indx_fd = open(argv[2], O_RDONLY) ) == -1){
		err(1, "ERROR: Opening file %s", argv[2]);
	}

	struct 

	if(){
		errx(2, "ERROR: %s format", argv[2]);
	}


	int bytes_count = 0;
	struct pos buf;

	while( (bytes_count = read(idx_fd, &buf, sizeof(buf)) ) > 0 )
	{
		if(:split
	}

	exit(0);
}
