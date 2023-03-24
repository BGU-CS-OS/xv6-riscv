#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    write(1, "Hello World!", strlen("Hello World!"));
    write(1, "\n", 1);
    exit(0);
}