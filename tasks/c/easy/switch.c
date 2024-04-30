#include <fcntl.h> // open
#include <stdio.h> // printf
#include <errno.h> // errno
#include <stdlib.h> // exit
int main() {
int result = open("/tmp/some_file", O_RDONLY);
if (result < 0) {
switch (errno) {
case ENOENT: printf("no such file\n"); break;
case EACCES: printf("permission denied\n"); break;
// ...
}
exit(1);
}
printf("opened /tmp/some_file successfully\n");
}
