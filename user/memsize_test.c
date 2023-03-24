// Test the memsize system call.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
memsizeTest(void)
{

  printf("%d\n", memsize());

  void *ptr = malloc(20000);
  printf("%d\n", memsize());

  free(ptr);
  
  printf("%d\n", memsize());
}

int
main(void)
{
  memsizeTest();
  exit(0);
}
