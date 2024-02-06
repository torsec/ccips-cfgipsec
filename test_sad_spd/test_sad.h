#include <unistd.h>
#include "sad_entry.h"
#include "log.h"
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include "parson.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "messages.h"
#include "trust_client.h"
#include "trust_handler.h"
#include "sysrepo_entries.h"

void 
fill_test_sad(sad_entry_node *sad_node);

void
test_sad();