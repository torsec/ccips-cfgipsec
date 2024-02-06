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
#include "sysrepo_entries.h"
// #include "constants.h"
// #include "base/serializers/sad_serializer.h"
// #include "base/serializers/spd_serializer.h"
#define VERSION "2"

void 
fill_test_sad(sad_entry_node *sad_node) {
    unsigned long long int req_id = 100;

    char *name = "test_sad";
    char local_subnet[MAX_IP] = "10.0.0.0/24";
    char remote_subnet[MAX_IP] = "11.0.0.0/24";
    char tunnel_local[MAX_IP] = "10.0.0.61";
    char tunnel_remote[MAX_IP] = "10.0.0.228";

    // Setup the structure
    
    // First setup the identification variables
    strcpy(sad_node->name,name);
    sad_node->req_id = req_id;
    // To verifify the use of this values
    sad_node->ext_seq_num = false;
    sad_node->seq_overflow = false;
    sad_node->spi = 11;
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
    sad_node->ipsec_mode = IPSEC_MODE_TUNNEL;
    sad_node->protocol_parameters = IPPROTO_ESP;
    // sad_node->protocol_parameters = 50;
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
    

    // this material should be sent encrypted to the Trusted Application(?)
    char* key = bytes;
    sad_node->encryption_alg = SADB_EALG_3DESCBC;
    sad_node->encryption_key = key;
    sad_node->integrity_key = key;
	sad_node->encryption_iv = key;
    sad_node->encryption_key_length = strlen(key);

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
}

void 
test_sad() {

    printf("------------------------------------------\n");
    printf("\n\t\tSAD TEST\n");
    printf("------------------------------------------\n");
    sleep(1);
    printf("TEST 1: Add trusted sad entry.\n");
    struct sad_entry_node *sad_node = create_sad_node();
    sad_entry_node *rec_sad = (sad_entry_node *)malloc(sizeof(sad_entry_node));
    printf("\tFirst step: create sad node data structure to send (assuming it has been already received by sysrepo)\n");
    fill_test_sad(sad_node);
    // printf("INPUT SAD NODE before add_trusted_sad_entry:\n");
    // printf("local subnet = %s\n", sad_node->local_subnet);
    // printf("encryption key = %s\n", sad_node->encryption_key);
    sleep(1);
    printf("\tSecond step: call add_trusted_sad_entry function to send the new sad entry to the trusted part.\n");
    add_trusted_sad_entry(rec_sad, sad_node);
    // after add_trusted_sad_entry() is called, the "encryption key" field of the sad is empty
    // printf("INPUT SAD NODE after add_trusted_sad_entry:\n");
    // printf("local subnet = %s\n", sad_node->local_subnet);
    // printf("encryption key = %s\n", sad_node->encryption_key);
    // printf("integrity key = %s\n", sad_node->integrity_key);
    // printf("ipsec_mode = %d\n", sad_node->ipsec_mode);
    sleep(2);
    printf("\tThird step: call pf_addsad function to install the new sad entry in the untrusted kernel through PFKEY API.\n");
    pf_addsad(rec_sad);
    // printf("INPUT SAD NODE after pf_addsad:\n");
    // printf("local subnet = %s\n", sad_node->local_subnet);
    // printf("encryption key = %s\n", sad_node->encryption_key);
    sleep(2);
    pf_dump_sads(sad_node);
    // printf("OUTPUT SAD NODE (after pf_getsad):\n");
    // printf("local subnet = %s\n", rec_sad->local_subnet);
    // printf("encryption key = %s\n", rec_sad->encryption_key);
    sleep(2);
    printf("\n\nTEST 2: Verify sad node.\n");
    printf("\tFirst step: retrieve the sad entry from the untrusted kernel through pf_getsad function.\n");
    if(pf_getsad(sad_node, rec_sad) !=0) {
        ERR("An error has ocurred");
    }
    // verify_sad_nodes(); // this function needs sysrepo connection
    char verify_response[32];
    sleep(1);
    printf("\tSecond step: call the function verify_trusted_sad_entry.\n");
    int verification = verify_trusted_sad_entry(verify_response, rec_sad);

    switch (verification)
			{
			case 1:
				// Socket error 
				break;
			case 2:
				ERR("ALERT with %s", verify_response);
				ERR("Invalid verification of %s: SPI %d\tREQID: %d",rec_sad->name,rec_sad->spi,rec_sad->req_id);
				break;
			case 3:
				ERR("INVALID ANSWER!");
				break;
			default:
				printf("\tCorrect verification of %s: SPI %d\t REQID: %d\n",rec_sad->name,rec_sad->spi,rec_sad->req_id);
				break;
			}


    printf("\n\nTEST 3: Delete trusted sad entry\n");
    printf("\tFirst step: retrieve from untrusted kernel the sad we want to delete.\n");
    printf("\tSecond step: call del_trusted_sad_entry to delete the sad from the trusted database.\n");
    del_trusted_sad_entry(rec_sad->name);
    sleep(1);
    printf("\tThird step: remove the sad from the untrusted kernel through pf_delsad function.\n");
    
    pf_delsad(rec_sad);
    
    sleep(2);
    
    return;
}


