// Functions needed by CCIPS for the Keystone version
// TODO add compile option Keystone as alternative to the Enarx one
// TODO adapt the verification process to the Keystone version
// TODO add the verification process for SPD

#include "../cmd/spirs_tee_sdk/host/tee_client_api/public/tee_client_api.h"

/**
 * @brief Invoked to start the Keystone enclave that runs the handle_message function
 * @param argv arguments needed to launch the enclave
 * @return 0 on success, -1 in case of errors
*/
int connect_ta_keystone();

/**
 * @brief Wait for enclave's thread termination
 * @param void
*/
void disconnect_ta_keystone();

/**
 * @brief Add sad_entry_node in the trusted list, it works like add_sad_node_enarx
 * @param node_entry sad_entry_node to be added in the trusted list
 * @return void
*/
void add_sad_node_keystone(sad_entry_node* node_entry);

/**
 * @brief Delete sad_entry_node from the trusted list, it works like del_sad_node_enarx
 * @param sad_name name of the sad_entry_node to be deleted from the trusted list
 * @return 0 on success, -1 in case of errors
*/
int del_sad_node_keystone(char *sad_name);

/**
 * @brief Add spd_entry_node in the trusted list, it works like add_sad_node_keystone but for policies
 * @param node_entry spd_entry_node to be added in the trusted list
 * @return void
*/
void add_spd_node_keystone(spd_entry_node* node_entry);

/**
 * @brief Delete spd_entry_node from the trusted list, it works like del_sad_node_keystone but for policies
 * @param spd_name name of the spd_entry_node to be deleted from the trusted list
 * @return 0 on success, -1 in case of errors
*/
int del_spd_node_keystone(char *spd_name);

/**
 * @brief Verify the spd_entry_node using the trusted list, it works like verify_trusted_sad_entry_keystone but for policies
 * @param alert Result of verification process
 * @param spd_node spd_entry_node to be compared with the trusted list's entry
 * @return 0 on success, TBD in case of errors
*/
int verify_trusted_spd_entry_keystone(char *alert, spd_entry_node *spd_node);

/**
 * @brief Verify the sad_entry_node using the trusted list, it works like verify_trusted_sad_entry
 * @param alert Result of verification process
 * @param sad_node sad_entry_node to be compared with the trusted list's entry
 * @return 0 on success, TBD in case of errors
*/
int verify_trusted_sad_entry_keystone(char *alert, sad_entry_node *sad_node);
