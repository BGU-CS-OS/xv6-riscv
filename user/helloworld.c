#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main()
{
    char* helloWorld = "Hello World xv6\n";
    write(1, helloWorld, strlen(helloWorld));
    exit(0, "status 0\n");
}
