#include <fcntl.h> // open
#include <stdio.h> // printf
#include <err.h> // err
int main() {
const char filename[] = "/tmp/some_file";
int result = open(filename, O_RDONLY);
if (result < 0) {
err(1, "could not open file %s", filename);
}
printf("opened %s successfully\n", filename);
}
