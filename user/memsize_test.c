#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main()
{
    int currSize = memsize();
    printf("Current size: %d\n", currSize);
    void *allocated = malloc(20 << 10);
    int newSize = memsize();
    printf("New size: %d\n", newSize);
    printf("Difference: %d\n", newSize - currSize);
    free(allocated);
    int finalSize = memsize();
    printf("Final size: %d\n", finalSize);
    exit(0, "status 0\n");
}