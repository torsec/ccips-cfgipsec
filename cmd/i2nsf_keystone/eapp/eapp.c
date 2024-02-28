#include "app/eapp_utils.h"
#include "string.h"
#include "edge/edge_call.h"
#include <syscall.h>
#include "trust_handler.h"
#include "log.h"
#include <stdio.h>

// EAPP_ENTRY bypass libc runtime
int main()
{
  printf("Hello world from Keystone enclave\n");

  EAPP_RETURN(16);
  return 0; // never executed
}

// extern char* handle_message_trusted(char* data) {
//     // function implementation
//     return handle_message(data);
// }


// int log_set_level_trusted(int level) {
//     // function implementation
//     log_set_level(level);
// }