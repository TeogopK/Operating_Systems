#include <fcntl.h> // open
#include <stdio.h> // printf
#include <err.h> // err
int main() {
int result = open("/tmp/some_file", O_RDONLY);
if (result < 0) {
err(1, "could not open file");
}
printf("opened /tmp/some_file successfully\n");
}
