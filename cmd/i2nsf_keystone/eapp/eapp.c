#include "app/eapp_utils.h"
#include "string.h"
#include "edge/edge_call.h"
#include <syscall.h>
#include "trust_handler.h"
#include "log.h"
#include <stdio.h>

#include "malloc.h"
#include "trust_handler.h"
#include "trust_client.h"
#include <string.h>

extern char* __malloc_start;
extern char* __malloc_zone_stop;

#define HEAP_SIZE ((char *)(&__malloc_zone_stop) - (char *)(&__malloc_start))

#define DELETE_MSG          \
"{\n"                       \
"    \"work_id\": 10,\n"    \
"    \"code\": 2,\n"        \
"    \"data\": {\n"         \
"        \"entry_id\": \"test1234\"\n"  \
"    }\n"                   \
"}"

void *
custom_malloc(size_t size) {
  void *p = malloc(size);
  printf("MALLOC - %lu: %p\n", size, p);
  return p;
}

void
custom_free(void *p) {
  printf("FREE: %p\n", p);
  free(p);
  return;
}

// EAPP_ENTRY bypass libc runtime
int main()
{
  // setup environment: set log level and initialize HEAP
  log_set_level(5);
  custom_heap_init(&__malloc_start, HEAP_SIZE);
  json_set_allocation_functions(custom_malloc, custom_free);

  // your code
  printf("Parsing string\n");
  JSON_Value *parsed_resp = json_parse_string(DELETE_MSG);
  JSON_Object *schema = json_object(parsed_resp);
  printf("Deallocating object\n");
  json_value_free(parsed_resp);
  printf("schema: %p\n", schema);

  fflush(stdout);

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