
#include "spd_entry.h"
#include <string.h>

#define MAX_PATH  200
#define MAX_IP 40
#define MAX_KEY 1024

spd_entry_node* create_spd_node(){
	spd_entry_node *spd_node = (spd_entry_node *) malloc(sizeof(struct spd_entry_node));
	spd_node->name = (char *) malloc(sizeof(char) * MAX_PATH);
	spd_node->index = 0;
    // Direction of the tunnel
	spd_node->policy_dir = 0;
	spd_node->req_id = 0;
	spd_node->local_subnet = (char *) malloc(sizeof(char) * MAX_IP); 
	spd_node->remote_subnet = (char *) malloc(sizeof(char) * MAX_IP); 
	spd_node->tunnel_local = (char *) malloc(sizeof(char) * MAX_IP); 
    spd_node->tunnel_remote = (char *) malloc(sizeof(char) * MAX_IP); 
    spd_node->inner_protocol = 0;
    spd_node->srcport = 0;
    spd_node->dstport = 0;
    spd_node->action = 0;
    spd_node->ext_seq_num =false;
    spd_node->seq_overflow = false;
    spd_node->ipsec_mode = 0;
    spd_node->protocol_parameters = 0;
    spd_node->integrity_alg = 0;
    spd_node->encryption_alg = 0;
    spd_node->anti_replay_window = 0;
    spd_node->pfp_flag = false;
    spd_node->stateful_frag_check = false;
    spd_node->bypass_dscp = false;
    spd_node->ecn = false;
    spd_node->tfc_pad = false;
    spd_node->df_bit = 0;
    spd_node->next=NULL;
    return spd_node;
}

void free_spd_node(spd_entry_node * n) {
	
    if (n != NULL) {
        if(n->name != NULL)
            free(n->name);
        if(n->local_subnet != NULL)
            free(n->local_subnet);
        if(n->remote_subnet != NULL)
            free(n->remote_subnet);
        if(n->tunnel_local != NULL)
            free(n->tunnel_local);
        if(n->tunnel_remote != NULL)
            free(n->tunnel_remote);  
        free (n);
    } 
}

void copy_spd_node(spd_entry_node *dst, spd_entry_node *src) {
    strcpy(dst->name, src->name);
    dst->index = src->index;
    dst->policy_dir = src->policy_dir;
    dst->req_id = src->req_id;
    strcpy(dst->local_subnet, src->local_subnet);
    strcpy(dst->remote_subnet, src->remote_subnet);
    strcpy(dst->tunnel_local, src->tunnel_local);
    strcpy(dst->tunnel_remote, src->tunnel_remote);
    dst->inner_protocol = src->inner_protocol;
    dst->srcport = src->srcport;
    dst->dstport = src->dstport;
    dst->action = src->action;
    dst->ext_seq_num = src->ext_seq_num;
    dst->seq_overflow = src->seq_overflow;
    dst->ipsec_mode = src->ipsec_mode;
    dst->protocol_parameters = src->protocol_parameters;
    dst->integrity_alg = src->integrity_alg;
    dst->encryption_alg = src->encryption_alg;
    dst->anti_replay_window = src->anti_replay_window;
    dst->pfp_flag = src->pfp_flag;
    dst->stateful_frag_check = src->stateful_frag_check;
    dst->bypass_dscp = src->bypass_dscp;
    dst->ecn = src->ecn;
    dst->tfc_pad = src->tfc_pad;
    dst->df_bit = src->df_bit;

    dst->next = src->next; // should be copied?

    return;
}

#ifdef Trusted

// https://github.com/kgabis/parson
JSON_Value *serialize_spd_node(spd_entry_node *spd_node) {
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = 	json_value_get_object(root_value);
    // char *serialized_string = NULL;

    // Initial vals
    json_object_set_string(root_object, "name", spd_node->name);
    json_object_set_number(root_object, "index", spd_node->index);
    json_object_set_number(root_object, "policy_dir", spd_node->policy_dir);
    json_object_set_number(root_object, "req_id", spd_node->req_id);
    json_object_set_string(root_object, "local_subnet", spd_node->local_subnet);
    json_object_set_string(root_object, "remote_subnet", spd_node->remote_subnet);
    json_object_set_string(root_object, "tunnel_local", spd_node->tunnel_local);
    json_object_set_string(root_object, "tunnel_remote", spd_node->tunnel_remote);
	json_object_set_number(root_object, "inner_protocol", spd_node->inner_protocol);
	json_object_set_number(root_object, "srcport", spd_node->srcport);
	json_object_set_number(root_object, "dstport", spd_node->dstport);
    json_object_set_number(root_object, "action", spd_node->action);
	json_object_set_boolean(root_object, "ext_seq_num", spd_node->ext_seq_num);
	json_object_set_boolean(root_object, "seq_overflow", spd_node->seq_overflow);
	json_object_set_number(root_object, "ipsec_mode", spd_node->ipsec_mode);
	json_object_set_number(root_object, "protocol_parameters", spd_node->protocol_parameters);
	json_object_set_number(root_object, "integrity_alg", spd_node->integrity_alg);
	json_object_set_number(root_object, "encryption_alg", spd_node->encryption_alg);
	json_object_set_number(root_object, "anti_replay_window", spd_node->anti_replay_window);
    json_object_set_boolean(root_object, "pfp_flag", spd_node->pfp_flag);
    json_object_set_boolean(root_object, "stateful_frag_check", spd_node->stateful_frag_check);
    json_object_set_boolean(root_object, "bypass_dscp", spd_node->bypass_dscp);
	json_object_set_boolean(root_object, "ecn", spd_node->ecn);
	json_object_set_boolean(root_object, "tfc_pad", spd_node->tfc_pad);
	json_object_set_number(root_object, "df_bit", spd_node->df_bit);
    // json_value_free(root_value);
    return root_value;
}

struct spd_entry_node *deserialize_spd_node(JSON_Object *schema) {
    // JSON_Object *schema = json_object(json_parse_string(serialized));
    spd_entry_node *spd_node = create_spd_node();
    strcpy(spd_node->name,json_object_get_string(schema, "name"));
    spd_node->index = json_object_get_number(schema, "index");
    spd_node->policy_dir = json_object_get_number(schema, "policy_dir");
    spd_node->req_id = json_object_get_number(schema, "req_id");
    strcpy(spd_node->local_subnet,json_object_get_string(schema, "local_subnet"));
    strcpy(spd_node->remote_subnet,json_object_get_string(schema, "remote_subnet"));
	spd_node->inner_protocol = json_object_get_number(schema, "inner_protocol");
	spd_node->srcport = json_object_get_number(schema, "srcport");
	spd_node->dstport = json_object_get_number(schema, "dstport");
  	spd_node->action = json_object_get_number(schema, "action");
	spd_node->seq_overflow = json_object_get_boolean(schema, "seq_overflow");
	spd_node->ipsec_mode = json_object_get_number(schema, "ipsec_mode");
	spd_node->protocol_parameters = json_object_get_number(schema, "protocol_parameters");
	spd_node->integrity_alg = json_object_get_number(schema, "integrity_alg");
	spd_node->encryption_alg = json_object_get_number(schema, "encryption_alg");
	spd_node->anti_replay_window = json_object_get_number(schema, "anti_replay_window");
	spd_node->pfp_flag = json_object_get_number(schema, "pfp_flag");
	spd_node->stateful_frag_check = json_object_get_number(schema, "stateful_frag_check");
	spd_node->bypass_dscp = json_object_get_boolean(schema, "bypass_dscp");
	spd_node->ecn = json_object_get_boolean(schema, "ecn");
	spd_node->tfc_pad = json_object_get_boolean(schema, "tfc_pad");
	spd_node->df_bit = json_object_get_number(schema, "df_bit");
    return spd_node;
}   

#endif