#include <fcntl.h> // open, close
#include <err.h> // err

int main()
{
	char buf[] = "Hello world!\n";
	int num_bytes = write(fd, buf, strlen(buf));

	if (num_bytes < 0) {
		err(1, "could not write data");
	}
	if (num_bytes != 13) {
		errx(1, "could not write data all at once");
	}
}
