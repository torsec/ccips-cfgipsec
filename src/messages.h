#ifndef __MESSAGES
#define __MESSAGES

#include "parson.h"
#include "sad_entry.h"




#define NEW_CONFIG_MSG        1 
#define DELETE_CONFIG_MSG     2
#define ALERT_STATE_MSG       3
#define OP_RESULT_MSG         4
// #define REQUEST_entry_MSG     5 For the moment not used.
#define INSERT_ENTRY_MSG      6
#define RETURN_entry_MSG      7 
#define REQUEST_verify_MSG    8
#define ERROR_MSG             -1


#define HASH_MAP_SIZE         16

// default_msg Message format used to share information between the nodes
typedef struct default_msg {
    int work_id;
    int code;
    JSON_Object *data;
} default_msg;



// sad_entry_msg Message used to share the new spa_entry to the trusted part
typedef struct sad_entry_msg {
    char entry_id[16];
    sad_entry_node *sad_entry;
} sad_entry_msg;

// delet_config_msg Message used to delete a set of entry from the trusted part.
typedef struct delete_config_msg {
    char entry_id[16];
} delete_config_msg;

// alert_state_msg
typedef struct alert_state_msg {
    char entry_id[16];
    char message[32];
} alert_state_msg;

// op_result_mgs;
typedef struct op_result_msg {
    int success; // 0 Success / 1 Fail
    char message[32];
} op_result_msg;

// request_entry_msg
typedef struct request_entry_msg {
    char entry_id[16];
} request_entry_msg;

typedef struct insert_entry_msg 
{
    char entry_id[16];
    sad_entry_node *sad_entry;
} insert_entry_msg;




int decode_default_msg(JSON_Object *schema, default_msg* msg);
char *encode_default_msg(int work_id, int code, JSON_Value *data);





// Functions called to decode the messages
int decode_sad_entry_msg(JSON_Object *schema, sad_entry_msg *msg);
int decode_delete_config_msg(JSON_Object *schema, delete_config_msg *msg);
int decode_alert_state_msg(JSON_Object *schema, alert_state_msg *msg);
int decode_op_result_msg(JSON_Object *schema, op_result_msg *msg);
int decode_request_entry_msg(JSON_Object *schema, request_entry_msg *msg);
int decode_insert_entry_msg(JSON_Object *schema, insert_entry_msg *msg);

// Functions to encode the messages
JSON_Value *encode_sad_entry_msg(sad_entry_msg *msg);
JSON_Value *encode_delete_config_msg(delete_config_msg *msg);
JSON_Value *encode_alert_state_msg(alert_state_msg *msg);
JSON_Value *encode_op_result_msg(op_result_msg *msg);
JSON_Value *encode_request_entry_msg(request_entry_msg *msg);
JSON_Value *encode_insert_entry_msg(insert_entry_msg *msg);

// Some helpers to remove redundancy
JSON_Value *generate_op_message(char* message, int code);



#endif