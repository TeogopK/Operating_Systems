#include <fcntl.h> // open, close
#include <err.h> // err

int main()
{
int fd = open("/tmp/some_file", O_RDONLY);
if (fd < 0) {
err(1, "could not open file");
}
char c;
int num_bytes;
while ((num_bytes = read(fd, &c, 1)) > 0) {
// < do something with the character in c >
}
if (num_bytes < 0) {
err(1, "could not read from file");
}
if (close(fd) < 0) {
err(1, "could not close file");
}
}
