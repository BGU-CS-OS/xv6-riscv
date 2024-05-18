#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main()
{
    void *p;

    printf("Before malloc: %d\n", memsize());

    p = malloc(20000);
    printf("After malloc: %d\n", memsize());
    
    free(p);
    printf("After free: %d\n", memsize());

    exit(0);
}