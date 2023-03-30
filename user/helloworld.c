#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main()
{
    write(1, "Hello, world!\n", strlen("Hello, world!\n"));
    exit(0, ":)");
}