#ifndef __TRUST_CLIENT
#define __TRUST_CLIENT

#include "sad_entry.h"
#include "spd_entry.h"
#include "log.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int connect_ta();
int disconnect_ta();
int add_trusted_sad_entry(sad_entry_node *new_sad, sad_entry_node *old_sad);
int del_trusted_sad_entry(sad_entry_node *sad_node);
int verify_trusted_sad_entry(char *alert_msg, sad_entry_node *sad_node);

#endif