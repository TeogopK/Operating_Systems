#include <fcntl.h> // open
#include <stdio.h> // printf
#include <stdlib.h> // exit
int main() {
int result = open("/tmp/some_file", O_RDONLY);
if (result < 0) {
printf("opening /tmp/some_file failed\n");
exit(1);
}
printf("opened /tmp/some_file successfully\n");
}
