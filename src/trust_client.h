#ifndef __TRUST_CLIENT
#define __TRUST_CLIENT

#ifdef __cplusplus
extern "C" {
#endif

#include "sad_entry.h"
#include "spd_entry.h"
#include "log.h"
#include "utils.h"
#include "keystone_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

struct keystone_ta_connect_ctx {
    TEEC_Result res;
    uint32_t eo;
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    const TEEC_UUID uuid;
};

/// @brief function to connect to the TA
/// @return 
int connect_ta();

/// @brief function that disconnects from the TA
/// @return 
int disconnect_ta();

/// @brief function that adds a new SAD entry to the TA
/// @param new_sad 
/// @param old_sad // We may need to switch inputs
/// @return 
int add_trusted_sad_entry(sad_entry_node *new_sad, sad_entry_node *old_sad);

/// @brief function that adds a new SAD entry to the Keystone TA
/// @param new_sad 
/// @param old_sad
/// @return 
int add_trusted_sad_entry_keystone(sad_entry_node *new_sad, sad_entry_node *old_sad);

/// @brief function that deletes a sad entry 
/// @param sad_node 
/// @return 
int del_trusted_sad_entry(char *sad_name);

/// @brief function that deletes a sad entry in the Keystone TA
/// @param sad_node 
/// @return 
int del_trusted_sad_entry_keystone(char *sad_name);

/// @brief function that verifies a SAD entry
/// @param alert_msg 
/// @param sad_node 
/// @return 
int verify_trusted_sad_entry(char *alert_msg, sad_entry_node *sad_node);


int add_trusted_spd_entry(spd_entry_node *new_spd, spd_entry_node *old_spd);

/// @brief function that adds a new SAD entry to the Keystone TA
/// @param new_spd 
/// @param old_spd
/// @return 
int add_trusted_spd_entry_keystone(spd_entry_node *new_spd, spd_entry_node *old_spd);

// @brief function that deletes a SAD entry from the Keystone TA
/// @param spd_name 
/// @return 
int del_trusted_spd_entry_keystone(char *spd_name);


int verify_trusted_spd_entry(char *alert, spd_entry_node *spd_node);
int del_trusted_spd_entry(char *spd_name);

#ifdef __cplusplus
}
#endif

#endif