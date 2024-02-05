#include "app/eapp_utils.h"
#include "string.h"
#include "edge/edge_call.h"
#include <syscall.h>

#include <stdio.h>

// EAPP_ENTRY bypass libc runtime
int main()
{
  printf("hello, world!\n");

  EAPP_RETURN(16);
  return 0; // never executed
}
