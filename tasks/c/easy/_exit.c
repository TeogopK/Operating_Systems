#include <unistd.h> // _exit
void foo(void) {
_exit(42); // exit status 42
}
int main(void)
{
foo();
}
