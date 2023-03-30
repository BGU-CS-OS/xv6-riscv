#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main()
{
	printf("1 - %d\n", memsize());
	long *i = malloc(20000);
	printf("2 - %d\n", memsize());
	free(i);
	printf("3 - %d\n", memsize());
	exit(0, ":D");
}