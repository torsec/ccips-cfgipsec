#include "trust_handler.h"
map_struct trusted_map = NULL;

void init_map() {
    if (trusted_map == NULL) {
        trusted_map = map_create();
    } else {
        WARN("Map already started %d, %d", trusted_map, &trusted_map);
    }
}

map_struct get_trusted_map() {
    return trusted_map;
} 
extern char *handle_message(char *data) {

    default_msg *msg = malloc(sizeof(default_msg));
    int result = 0;
    int code = 0;
    JSON_Value *data_value;;
    JSON_Object *schema = json_object(json_parse_string(data));
    if (schema == NULL) {
        int result = -1;
        int code = -1;
        data_value = generate_op_message("WRONG JSON",-1);    
        goto cleanup;
    }
    // TODO handle error of decode_default
    if (schema == NULL || decode_default_msg(msg, schema) != 0) {
        // TODO handle error of decode_default
        goto cleanup;
    } 
    init_map();

    switch (msg->code) {
        case NEW_CONFIG_MSG: {
            sad_entry_msg *entry_msg = (sad_entry_msg*) malloc(sizeof(sad_entry_msg));    
            if ((result = handle_new_conf_message(msg->data,entry_msg)), result != 0) {
                free(entry_msg);
                data_value = generate_op_message("newconf err",result);
                code = OP_RESULT_MSG;
            } else {
                data_value = encode_sad_entry_msg(entry_msg);
                code = INSERT_ENTRY_MSG;
                INFO("NEW CONFIG MANAGED SUCCESFUL");
            }
            break;
        }
        case REQUEST_VERIFY_MSG: {
            alert_state_msg *alert_msg = (alert_state_msg*) malloc(sizeof(alert_state_msg));    
            if (result = handle_request_verify_message(msg->data,alert_msg), result == 0) {
                // The confirmation has been succesfull
                data_value = generate_op_message("SAD_ENTRY is valid",0);
                code = OP_RESULT_MSG;
                INFO("VERIFY MANAGED SUCCESFUL");
            } else if (result  == 2){
                data_value = encode_alert_state_msg(alert_msg);
                code = ALERT_STATE_MSG;
                ERR("VERIFY WAS UNCSUCCESFUL");
            } else {
                data_value = generate_op_message("Verify error",result);
                code = OP_RESULT_MSG;
            }
            free(alert_msg);
            break;
        }
        case DELETE_CONFIG_MSG: {
            op_result_msg *op_msg = (op_result_msg*) malloc(sizeof(op_result_msg));    
            if (result = handle_request_remove(msg->data,op_msg), result != 0) {
                ERR("Error deleting entry");
            } else {
                INFO("DELETE MANAGED SUCCESFUL");
            }
            data_value = encode_op_result_msg(op_msg);
            code = OP_RESULT_MSG;
            free(op_msg);
            break;
        }
    }
    char *out_data;
cleanup:
    out_data = encode_default_msg(msg->work_id,code,data_value);
    // free(msg);
    // return out_data;
    // if (data_value != NULL) {
    //     json_value_free(data_value);
    // }
    return out_data;
}


int handle_new_conf_message(JSON_Object *data, sad_entry_msg *out) {
    int status = 0;
    // Decode the data of the message
    sad_entry_msg *config = (sad_entry_msg*) malloc(sizeof(sad_entry_msg)); 
    if (decode_sad_entry_msg(config,data) != 0) {
        ERR("Error decoding the data of the message");
        status = 1;
        goto cleanup;
    }
    sad_entry_node *entry = (sad_entry_node*) malloc(sizeof(sad_entry_node));
    // Copy struct into another so we can free later the config value
    memcpy(&entry, &config->sad_entry, sizeof(config->sad_entry));
    // XOR the key parameters
    // TODO Add this part
    // Store the values
    char hash[HASH_MAP_SIZE];
    get_sad_hash(config->sad_entry,hash);
    if (m_set_sad_entry(trusted_map,hash,entry) != 0) {
        ERR("Error adding sad_entry to map");
        status =  1;
        goto cleanup;
    }
    // Free data
    strcpy(out->entry_id,config->entry_id);
    out->sad_entry = entry;
cleanup:
	free(config);
    return status;
}


int handle_request_verify_message(JSON_Object *data, alert_state_msg *out) {
    int status = 0;
    // Decode the data of the message
    sad_entry_msg *config = (sad_entry_msg*) malloc(sizeof(sad_entry_msg));
    if (decode_sad_entry_msg(config,data) != 0) {
        ERR("Error decoding the data of the message");
        status = 1;
        goto cleanup;
    }
    // Check if the node exists based in the hash
    char hash[HASH_MAP_SIZE];
    get_sad_hash(config->sad_entry,hash);

    // Check if hash is equal to entry_id
    if (sizeof(config->entry_id) != sizeof(hash) && strcmp(config->entry_id,hash) != 0) {
        status = 1;
        goto cleanup;
    }

    sad_entry_node *stored_entry = m_get_sad_entry(trusted_map,hash);
    if (stored_entry == NULL) {
        status = 1;
        goto cleanup;
    }
    // Is the same entry?
    if (compare_sad_entries(config->sad_entry,stored_entry) != 0) {
        // Generate out message
        strcpy(out->message, "entries differ");
        strcpy(out->entry_id,config->entry_id);
        status = 2;
        goto cleanup;
    }
cleanup:
	free(config);
    return status;
}


int handle_request_remove(JSON_Object *data, op_result_msg *out) {
    int status = 0;
    char message[16];
    // Decode the data of the message
    delete_config_msg *config = (delete_config_msg*) malloc(sizeof(delete_config_msg));
    if (decode_delete_config_msg(config,data) != 0) {
        ERR("Error decoding the data of the message");
        strcpy(message,"decoding\0");
        status = 1;
        goto cleanup;
    }
    // Is the hash HASH_MAP_SIZE bytes long
    if (sizeof(config->entry_id) != HASH_MAP_SIZE) {
        strcpy(message,"hash size\0");
        status = 1;
        goto cleanup;
    }

    // Does the sad entry exists?
    sad_entry_node *stored_entry = m_get_sad_entry(trusted_map,config->entry_id);
    if (stored_entry == 0) {
        strcpy(message,"do not exist\0");
        status = 1;
        goto cleanup;
    }
    strcpy(message,"deleted\0");
    // Delete the sad entry
    m_delete_sad_entry(trusted_map,config->entry_id);
cleanup:
	free(config);
    strcpy(out->message, message);
    out->success = status;
    return status;
}



