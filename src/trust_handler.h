#include "messages.h"
#include <stdio.h>
#include "log.h"
#include "map.h"
#include "utils.h"
#ifndef __TRUST_HANDLER
#define __TRUST_HANDLER



void init_map();
map_struct get_trusted_map();
int handle_new_conf_message (JSON_Object *data, sad_entry_msg *out);
int handle_request_verify_message(JSON_Object *data, alert_state_msg *out);
int handle_request_remove(JSON_Object *data, op_result_msg *out);
int handle_meesage(char *data, char *out);
extern char *handle_message(char *data);


#endif