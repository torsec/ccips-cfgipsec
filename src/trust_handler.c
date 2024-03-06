#include "trust_handler.h"

/************** Trusted Local Databases **************/

sad_entry_node *trusted_init_sad_node = NULL;
spd_entry_node *trusted_init_spd_node = NULL;

/*****************************************************/

extern char *handle_message(char *data) {
    // INFO("Input data for handle_message:\n%s", data); // json
    default_msg *msg = malloc(sizeof(default_msg));
    int result = 0, code = 0;
    char *out_data;
    JSON_Value *data_value;
    JSON_Value *parsed_data = json_parse_string(data);
    JSON_Object *schema = json_object(parsed_data);
    if (schema == NULL) {
        result = -1;
        code = -1;
        data_value = generate_op_message("WRONG JSON", -1);    
        goto cleanup; // msg is not initialized
    }

    // TODO handle error of decode_default
    if (schema == NULL || decode_default_msg(schema, msg) != 0) {
        // TODO handle error of decode_default
        goto cleanup;
    } 
    switch (msg->code) {
        case NEW_CONFIG_MSG: {
            sad_entry_msg *entry_msg = (sad_entry_msg*) malloc(sizeof(sad_entry_msg));    

            // TODO: modify handle 
            if ((result = handle_new_conf_message(msg->data,entry_msg)), result != 0) {
                data_value = generate_op_message("newconf err",result);
                code = OP_RESULT_MSG;
            } else {
                data_value = encode_sad_entry_msg(entry_msg);
                code = INSERT_ENTRY_MSG;
                INFO("NEW CONFIG MANAGED SUCCESFUL");
            }
            free(entry_msg);

            break;
        }
        case NEW_SPD_CONFIG_MSG: {
            spd_entry_msg *entry_msg = (spd_entry_msg*) malloc(sizeof(spd_entry_msg));    
            // TODO: modify handle 
            if ((result = handle_new_SPD_conf_message(msg->data,entry_msg)), result != 0) {
                data_value = generate_op_message("newconf SPD err",result);
                code = OP_SPD_RESULT_MSG;
            } else {
                data_value = encode_spd_entry_msg(entry_msg);
                code = INSERT_SPD_ENTRY_MSG;
                INFO("NEW SPD CONFIG MANAGED SUCCESFULLY");
            }
            free(entry_msg);

            break;
        }
        case REQUEST_VERIFY_MSG: {
            alert_state_msg *alert_msg = (alert_state_msg*) malloc(sizeof(alert_state_msg));
            alert_msg->entry_id = (char *) malloc(sizeof(char) * MAX_PATH); 

            if (result = handle_request_verify_message(msg->data,alert_msg), result == 0) {
                // The confirmation has been succesfull
                data_value = generate_op_message("SAD_ENTRY is valid",0);
                code = OP_RESULT_MSG;
                // INFO("VERIFY MANAGED SUCCESFUL");
            } else if (result  == 2){
                data_value = encode_alert_state_msg(alert_msg);
                code = ALERT_STATE_MSG;
                // Show the hash, reqid, and spi
                
            } else {
                data_value = generate_op_message("Verify error",result);
                code = OP_RESULT_MSG;
            }

            free(alert_msg->entry_id);
            free(alert_msg);
            break;
        }
        /*case REQUEST_SPD_VERIFY_MSG: {
            alert_state_msg *alert_msg = (alert_state_msg*) malloc(sizeof(alert_state_msg));
            alert_msg->entry_id = (char *) malloc(sizeof(char) * MAX_PATH); 

            if (result = handle_request_verify_SPD_message(msg->data,alert_msg), result == 0) {
                // The confirmation has been succesfull
                data_value = generate_op_message("SPD_ENTRY is valid",0);
                code = OP_SPD_RESULT_MSG;
                // INFO("VERIFY MANAGED SUCCESFUL");
            } else if (result  == 2){
                data_value = encode_alert_state_msg(alert_msg);
                code = ALERT_SPD_STATE_MSG;
                // Show the hash, reqid, and spi                
            } else {
                data_value = generate_op_message("Verify SPD error",result);
                code = OP_SPD_RESULT_MSG;
            }

            free(alert_msg);
            break;
        }*/
        case DELETE_CONFIG_MSG: {
            op_result_msg *op_msg = (op_result_msg*) malloc(sizeof(op_result_msg)); 

            // TODO: modify handle 
            if (result = handle_request_remove(msg->data,op_msg), result != 0) {
                ERR("Error deleting entry");
            } else {
                INFO("DELETE MANAGED SUCCESFUL");
            }

            data_value = encode_op_result_msg(op_msg);
            code = OP_RESULT_MSG;
            // [updated, to check] TODO revise this free, since sometimes we get some errors with wasm and the application crashes
            // only when forcing the delete from the controller. When removing through the rekey phase it works without any issue
            free(op_msg);
            // INFO("FEE_OP_MSG");
            break;
        }
        case DELETE_SPD_CONFIG_MSG: {
            op_result_msg *op_msg = (op_result_msg*) malloc(sizeof(op_result_msg)); 

            // TODO: modify handle 
            if (result = handle_request_remove_SPD(msg->data,op_msg), result != 0) {
                ERR("Error deleting SPD entry");
            } else {
                INFO("DELETE SPD MANAGED SUCCESFUL");
            }

            data_value = encode_op_result_msg(op_msg);
            code = OP_SPD_RESULT_MSG;
            // [updated, to check] TODO revise this free, since sometimes we get some errors with wasm and the application crashes
            // only when forcing the delete from the controller. When removing through the rekey phase it works without any issue
            free(op_msg);
            // INFO("FEE_OP_MSG");
            break;
        }
    }

cleanup:
    out_data = encode_default_msg(msg->work_id,code,data_value);
    free(msg);
    json_value_free(parsed_data);
    return out_data;
}

// TODO change order of input parameters
int handle_new_conf_message(JSON_Object *data, sad_entry_msg *out) {
    
    int status = 0;
    // Decode the data of the message
    sad_entry_msg *config = (sad_entry_msg*) malloc(sizeof(sad_entry_msg)); 
    if (decode_sad_entry_msg(data, config) != 0) {
        ERR("Error decoding the data of the message");
        status = 1;
        goto cleanup;
    }

    // This may be optimized
    sad_entry_node *entry = create_sad_node();
    copy_sad_node(entry, config->sad_entry);
    INFO("received SAD NODE:\nencryption key = %s\nlocal subnet = %s\n",entry->encryption_key, entry->local_subnet);
    // XOR the key parameters
    // TODO Add this part
    // Store the values
    
    if (get_sad_node(&trusted_init_sad_node,entry->name) != NULL) {
        ERR("Error adding sad_entry, it already exists");
        status =  1;
        free_sad_node(entry);
        goto cleanup;
    }

    // TODO Proceed with the decryption of the entry

    if (add_sad_node(&trusted_init_sad_node,entry) != 0) {
        ERR("Error adding sad_entry, it already exists");
        status =  1;
        free_sad_node(entry);
        goto cleanup;
    }


    // strcpy(out->entry_id,hash);
    out->sad_entry = entry; // it's the same value stored in the trusted list
    INFO("Added SAD entry: HASH: %s \t SPI: %d \t REQID: %d",
    entry->name,entry->spi,entry->req_id);
cleanup:
    // Free data
    free_sad_node(config->sad_entry);
	free(config);
    return status;
}

int handle_new_SPD_conf_message(JSON_Object *data, spd_entry_msg *out) {
    int status = 0;

    // Decode the data of the message
    spd_entry_msg *config = (spd_entry_msg*) malloc(sizeof(spd_entry_msg)); 
    if (decode_spd_entry_msg(data, config) != 0) {
        ERR("Error decoding the data of the SPD message");
        status = 1;
        goto cleanup;
    }

    // This may be optimized
    spd_entry_node *entry = create_spd_node();
    copy_spd_node(entry, config->spd_entry);
    INFO("entry->name = %s", entry->name);

    if (get_spd_node(&trusted_init_spd_node, entry->name) != NULL) {
        ERR("Error adding spd_entry, it already exists");
        status =  1;
        free_spd_node(entry);
        goto cleanup;
    }

    if (add_spd_node(&trusted_init_spd_node,entry) != 0) {
        ERR("Error adding spd_entry, it already exists");
        status =  1;
        free_spd_node(entry);
        goto cleanup;
    }

    // strcpy(out->entry_id,hash);
    out->spd_entry = entry;
    INFO("Added SPD entry: HASH: %s \t REQID: %d", entry->name,entry->req_id);
cleanup:
    // Free data
    free_spd_node(config->spd_entry);
	free(config);
    return status;
}


int handle_request_verify_message(JSON_Object *data, alert_state_msg *out) {
    int status = 0;
    // Decode the data of the message
    sad_entry_msg *config = (sad_entry_msg*) malloc(sizeof(sad_entry_msg));

    if (decode_sad_entry_msg(data, config) != 0) {
        ERR("Error decoding the data of the message");
        status = 1;
        goto cleanup;
    }

    // Check if hash is equal to entry_id Do no check for this since this needs to be calculated by the trusted app
    // if (sizeof(config->entry_id) != sizeof(hash) && strcmp(config->entry_id,hash) != 0) {
    //     ERR("Hash not equal");
    //     status = 1;
    //     goto cleanup;
    // }
    sad_entry_node *received_entry = config->sad_entry;
    sad_entry_node *stored_entry = get_sad_node(&trusted_init_sad_node,received_entry->name);
    if (stored_entry == NULL) {
        ERR("Entry not found");
        status = 1;
        goto cleanup;
    }
    INFO("STORED key = %s", stored_entry->encryption_key);
    INFO("RECEIVED key = %s", received_entry->encryption_key);
    // Is the same entry?
    if (compare_sad_entries(received_entry, stored_entry) != 0) {
        // Generate out message
        strcpy(out->message, "entries differ");
        strcpy(out->entry_id, received_entry->name);
        status = 2;
        char *stor_int_key_b = stringToBytes(stored_entry->integrity_key);
        char *recv_int_key_b = stringToBytes(received_entry->integrity_key);
        char *stor_enc_key_b = stringToBytes(stored_entry->encryption_key);
        char *recv_enc_key_b = stringToBytes(received_entry->encryption_key);
        ERR("Entry could not be validated: Name: %s\tSPI: %d\tREQID: %d", stored_entry->name, stored_entry->spi, stored_entry->req_id);
        ERR("\n\tStored AUTH_KEY: %s \t Current AUTH_KEY: %s \n\tStored ENC_KEY: %s \t Current ENC_KEY: %s", stor_int_key_b, recv_int_key_b, stor_enc_key_b, recv_enc_key_b);
        free(stor_int_key_b);
        free(recv_int_key_b);
        free(stor_enc_key_b);
        free(recv_enc_key_b);
        goto cleanup;
    } else {
        INFO("Entry validated: Name: %s\tSPI: %d\tREQID: %d",stored_entry->name, stored_entry->spi,stored_entry->req_id);
    }
cleanup:
    free_sad_node(config->sad_entry);
	free(config);
    return status;
}

/*int handle_request_verify_SPD_message(JSON_Object *data, alert_state_msg *out) {
    int status = 0;
    // Decode the data of the message
    spd_entry_msg *config = (spd_entry_msg*) malloc(sizeof(spd_entry_msg));

    if (decode_spd_entry_msg(data, config) != 0) {
        ERR("Error decoding the data of the SPD message");
        status = 1;
        goto cleanup;
    }

    // Check if hash is equal to entry_id Do no check for this since this needs to be calculated by the trusted app
    // if (sizeof(config->entry_id) != sizeof(hash) && strcmp(config->entry_id,hash) != 0) {
    //     ERR("Hash not equal");
    //     status = 1;
    //     goto cleanup;
    // }
    spd_entry_node *received_entry = config->spd_entry;
    spd_entry_node *stored_entry = get_spd_node(&trusted_init_spd_node, received_entry->name);
    if (stored_entry == NULL) {
        ERR("Entry not found");
        status = 1;
        goto cleanup;
    }
    
    // It is the same entry?
    if (compare_sad_entries(config->sad_entry,stored_entry) != 0) {
        // Generate out message
        strcpy(out->message, "entries differ");
        strcpy(out->entry_id,config->spd_entry->name);
        status = 2;
        ERR("Entry could not be validated: Name: %s\tSPI: %d\tREQID: %d", stored_entry->name, stored_entry->spi, stored_entry->req_id);
        ERR("\n\tStored AUTH_KEY: %s \t Current AUTH_KEY: %s \n\tStored ENC_KEY: %s \t Current ENC_KEY: %s",stringToBytes(stored_entry->integrity_key),stringToBytes(received_entry->integrity_key),stringToBytes(stored_entry->encryption_key),stringToBytes(received_entry->encryption_key)); // fix string deallocation
        goto cleanup;
    } else {
        INFO("Entry validated: Name: %s\tSPI: %d\tREQID: %d",stored_entry->name, stored_entry->spi,stored_entry->req_id);
    }
cleanup:
	free(config);
    return status;
}*/

int handle_request_remove(JSON_Object *data, op_result_msg *out) {
    int status = 0;
    char message[16];
    // Decode the data of the message
    delete_config_msg *config = (delete_config_msg*) malloc(sizeof(delete_config_msg));
    config->entry_id = (char *)malloc(sizeof(char) * MAX_PATH);
    if (decode_delete_config_msg(data, config) != 0) {
        ERR("Error decoding the data of the message");
        strcpy(message,"decoding\0");
        status = 1;
        goto cleanup;
    }

    // Does the sad entry exists?
    sad_entry_node *stored_entry = get_sad_node(&trusted_init_sad_node,config->entry_id);
    if (stored_entry == NULL) {
        ERR("SAD entry with id %s does not exists",config->entry_id);
        strcpy(message,"do not exist\0");
        status = 1;
        goto cleanup;
    }
    
    strcpy(message,"deleted\0");
    // Delete the sad entry
    INFO("Deleted SAD entry: Name: %s \t SPI: %d \t REQID: %d", config->entry_id, stored_entry->spi, stored_entry->req_id);
    del_sad_node(&trusted_init_sad_node,config->entry_id);

cleanup:
    free(config->entry_id);
	free(config);
    strcpy(out->message, message);
    out->success = status;
    return status;
}

int handle_request_remove_SPD(JSON_Object *data, op_result_msg *out) {
    int status = 0;
    char message[16];

    show_spd_list(trusted_init_spd_node);
    // Decode the data of the message
    delete_config_msg *config = (delete_config_msg*) malloc(sizeof(delete_config_msg));
    config->entry_id = (char *)malloc(sizeof(char) * MAX_PATH);
    if (decode_delete_config_msg(data, config) != 0) {
        ERR("Error decoding the data of the SPD message");
        strcpy(message,"decoding\0");
        status = 1;
        goto cleanup;
    }
    DBG("ENTRY ID = %s", config->entry_id);
    // Does the SPD entry exists?
    spd_entry_node *stored_entry = get_spd_node(&trusted_init_spd_node, config->entry_id);
    if (stored_entry == NULL) {
        ERR("SPD entry with id %s does not exists", config->entry_id);
        strcpy(message,"do not exist\0");
        status = 1;
        goto cleanup;
    }
    
    strcpy(message,"deleted\0");
    del_spd_node(&trusted_init_spd_node, config->entry_id);
    INFO("Deleted SPD entry: Index: %s \t POLICY DIR: %d \t REQID: %d", config->entry_id, stored_entry->policy_dir, stored_entry->req_id);

cleanup:
    free(config->entry_id);
	free(config);
    strcpy(out->message, message);
    out->success = status;
    return status;
}
