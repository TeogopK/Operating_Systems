#include <unistd.h> // getuid, geteuid
#include <sys/types.h> // uid_t
#include <stdio.h> // printf
int main(void)
{
uid_t me = getuid();
uid_t pretending = geteuid();
printf("uid: %d euid: %d\n", me, pretending);
return 0; // exit status 0
}
