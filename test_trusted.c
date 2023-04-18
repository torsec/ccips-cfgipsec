#include <unistd.h>
// #include "utils.h"
// #include "constants.h"
// #include "base/log.h"
// #include "base/pfkeyv2_entry.h"
#include "sad_entry.h"
#include "spd_entry.h"
#include "log.h"
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include "parson.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "messages.h"
#include "trust_client.h"
#include "trust_handler.h"
// #include "constants.h"
// #include "base/serializers/sad_serializer.h"
// #include "base/serializers/spd_serializer.h"

int exit_application = 0;

// sudo gcc -pthread -g -w -I /usr/include/libnl3/ -o test test.c base/* parson/parson.h parson/parson.c
static void
sigint_handler(int signum)
{
    (void)signum;

    exit_application = 1;
}


int 
main(int argc, char **argv) {


 connect_ta();
//  disconnect_ta();

//  int sock = socket(AF_INET, SOCK_STREAM, 0);
//     if (sock < 0) {
//         perror("socket failed");
//         exit(EXIT_FAILURE);
//     }

//     struct sockaddr_in server_addr;
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
//     server_addr.sin_port = htons(10000);

//     if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
//         perror("connect failed");
//         exit(EXIT_FAILURE);
//     }


//     char buffer[2048] = {0};
//     if (recv(sock, buffer, 2048, 0) < 0) {
//         perror("recv failed");
//         exit(EXIT_FAILURE);
//     }

//     printf("Received response from server: %s\n", buffer);



    // pf_exec_register(SADB_SATYPE_ESP);    
    unsigned long long int req_id = 100;

    char *name = "aaa";
    char local_subnet[MAX_IP] = "10.0.0.0/24";
    char remote_subnet[MAX_IP] = "11.0.0.0/24";
    char tunnel_local[MAX_IP] = "10.0.0.61";
    char tunnel_remote[MAX_IP] = "10.0.0.228";

	spd_entry_node *spd_node = create_spd_node();

    strcpy(spd_node->name,name);
    spd_node->policy_dir = IPSEC_DIR_OUTBOUND;
	spd_node->req_id=req_id;

    // Setup subnets
    // Local subnet is the subnet we want to intercconnect
    strcpy(spd_node->local_subnet,local_subnet);
    // Remote subnet is the other subnet of the tunnel that we want to interconnect
    strcpy(spd_node->remote_subnet,remote_subnet);
    // Tunnel local is the ip of the ip used to create the tunnel
    strcpy(spd_node->tunnel_local,tunnel_local);
    // Tunnel remote is the ip exposed by the other end to stablish the tunnel
    strcpy(spd_node->tunnel_remote,tunnel_remote);
    // Protocol we are encapsulating 256 stands for any
    spd_node->inner_protocol = 256;
    // We are not using ports so we set them to 0
    spd_node->srcport = 0;
    spd_node->dstport = 0;

	spd_node->action = IPSEC_POLICY_PROTECT;


    spd_node->ext_seq_num = false;
    spd_node->seq_overflow = false;

    // IPsec mode, we are running this as a tunnel, we setup protocol_params as ESP
    spd_node->ipsec_mode = IPSEC_MODE_TUNNEL;
    // spd_node->protocol_parameters = IPPROTO_ESP;
    spd_node->protocol_parameters = 50;

    // Algorithms configuration (Some random values)
    spd_node->integrity_alg = SADB_AALG_SHA1HMAC;
    spd_node->encryption_alg = SADB_EALG_3DESCBC;

        // TODO understand what those values do
    spd_node->bypass_dscp = false;
    spd_node->ecn = false;
    spd_node->tfc_pad = false;
    spd_node->pfp_flag= false;
    // DF BIT?
    spd_node->df_bit = 0;
    int rc;
   


    struct sad_entry_node *sad_node = create_sad_node();
    struct sad_entry_node *sad_node_get;

    // Setup the structure
    
    // First setup the identification variables
    strcpy(sad_node->name,name);
    sad_node->req_id = req_id;
    // To verifify the use of this values
    sad_node->ext_seq_num = false;
    sad_node->seq_overflow = false;
    sad_node->spi = 0;
	sad_node->seq_number_counter = 0;
	sad_node->anti_replay_window = 0;

    // Setup subnets
    // Local subnet is the subnet we want to intercconnect
    strcpy(sad_node->local_subnet,local_subnet);
    // Remote subnet is the other subnet of the tunnel that we want to interconnect
    strcpy(sad_node->remote_subnet,remote_subnet);
    // Tunnel local is the ip of the ip used to create the tunnel
    strcpy(sad_node->tunnel_local,tunnel_local);
    // Tunnel remote is the ip exposed by the other end to stablish the tunnel
    strcpy(sad_node->tunnel_remote,tunnel_remote);
    // Protocol we are encapsulating 256 stands for any
    sad_node->inner_protocol = 256;
    // We are not using ports so we set them to 0
    sad_node->srcport = 0;
    sad_node->dstport = 0;
    // IPsec mode, we are running this as a tunnel, we setup protocol_params as ESP
    sad_node->ipsec_mode = IPSEC_MODE_TRANSPORT;
    // sad_node->protocol_parameters = IPPROTO_ESP;
    sad_node->protocol_parameters = 50;
    // Algorithms configuration (Some random values)
    sad_node->integrity_alg = SADB_AALG_SHA1HMAC;
// example input hex string
    const char* hexstr = "af:6a:40:4c";
    // convert the hex string to a byte array
    char bytes[256];

    remove_colon(bytes, hexstr);
    if (bytes == NULL) {
        printf("Invalid input hex string\n");
        return 1;
    }
    
    // print the output byte array
    for (size_t i=0; bytes[i] != '\0'; i++) {
        printf("%02x ", bytes[i]);
    }
    printf("\n");

    char* key = bytes;
    sad_node->encryption_alg = SADB_EALG_3DESCBC;
    sad_node->encryption_key = key;
    sad_node->integrity_key = key;
	sad_node->encryption_iv = key;

    // TODO understand what those values do
    sad_node->bypass_dscp = false;
    sad_node->ecn = false;
    sad_node->tfc_pad = false;

    // DF BIT?
    sad_node->df_bit = 0;
	
    
    // Lifetime stuff
	sad_node->lft_bytes_hard= 10;
	sad_node->lft_bytes_soft= 1000;
	sad_node->lft_bytes_current= 1000;
	sad_node->lft_packets_hard= 1000;
	sad_node->lft_packets_soft= 1000;
	sad_node->lft_packets_current= 1000;
	sad_node->lft_time_hard= 1000;
	sad_node->lft_time_soft= 1000;
	sad_node->lft_time_current= 10;
	sad_node->lft_idle_hard= 60;
	sad_node->lft_idle_soft= 10;
	sad_node->lft_idle_current= 10;


    // // if (!cser_raw_store_struct_spd_entry_node(spd_node,))  
    // sad_entry_msg *message = (sad_entry_msg*) malloc(sizeof(sad_entry_msg)); 
    // message->sad_entry =  sad_node;

    // JSON_Value *new_conf_msg = encode_sad_entry_msg(message);
    // char *serialized_msg = encode_default_msg(10,NEW_CONFIG_MSG,new_conf_msg);
    sad_entry_node *rec_sad = (sad_entry_node*) malloc(sizeof(sad_entry_node)); 
    if (add_sad_entry(rec_sad,sad_node) != 0) {
        printf("Error when adding\n");
    } else {
        printf("Added Sad entry\n");
    }

    char verify_response[32];
    rec_sad->integrity_key = "aaaaaaaa";
    rec_sad->encryption_key = "aaaaaaaa";
    int verification = verify_sad_entry(verify_response,rec_sad);
    switch (verification)
    {
    case 1:
        // Socket error 
        break;
    case 2:
        ERR("ALERT: %s", verify_response);
        break;
    case 3:
        ERR("INVALID ANSWER!");
        break;
    default:
        INFO("Verification has been done correctly");
        break;
    }
    


    if (del_sad_entry(rec_sad) != 0) {
        printf("Error when deleting\n");
    } else {
        printf("Deleted Sad entry\n");
    }

    // if (send(sock, serialized_msg, strlen(serialized_msg), 0) < 0) {
    //     perror("send failed");
    //     exit(EXIT_FAILURE);
    // }
    // // send(sock, buffer, strlen(buffer), 0);


    // char buffer2[2048] = {0};
    // if (recv(sock, buffer2, 2048, 0) < 0) {
    //     perror("recv failed");
    //     exit(EXIT_FAILURE);
    // }

    // printf("Received response from server: %s\n", buffer2);

    // // Now parse the answer 
 
    // JSON_Value *data_value;
    // default_msg *msg = malloc(sizeof(default_msg));
    // JSON_Object *schema = json_object(json_parse_string(buffer2));
    // if (schema == NULL) {
    //     int result = -1;
    //     int code = -1;
    //     data_value = generate_op_message("WRONG JSON",-1);    
    //     //goto cleanup;
    //     exit(1);
    // }

    // if (schema == NULL || decode_default_msg(schema,msg) != 0) {
    //     // TODO handle error of decode_default
    //     //goto cleanup;
    //     exit(1);
    // }

    // int result = 0;

    // sad_entry_node *sad_return; 
    // switch (msg->code) {
    //     case INSERT_ENTRY_MSG: {
    //         sad_entry_msg *config = (sad_entry_msg*) malloc(sizeof(sad_entry_msg)); 
    //         sad_entry_msg *entry_msg = (sad_entry_msg*) malloc(sizeof(sad_entry_msg)); 
    //         if ((result = handle_new_conf_message(msg->data,entry_msg)), result != 0) {
    //             //goto cleanup;
    //             exit(1);
    //         }
    //         sad_return = entry_msg->sad_entry;
    //         break;
    //     }
    //     case OP_RESULT_MSG: {
    //         op_result_msg *result = (op_result_msg*) malloc(sizeof(op_result_msg)); 
    //         if ((result = decode_op_result_msg(msg->data,result)), result != 0) {
    //             //goto cleanup;
    //             exit(1);
    //         }
    //         if (result->success != 0) {
    //              printf("Error when adding %s.\n",result);
    //             exit(1);
    //         }
    //     }
    // }
    printf("Application exit requested, exiting.\n");
    
    return 0;
}


