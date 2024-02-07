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


void
fill_test_spd(spd_entry_node *spd_node) {
    unsigned long long int req_id = 111;

    char *name = "test_spd";
    char local_subnet[MAX_IP] = "10.0.0.0/24";
    char remote_subnet[MAX_IP] = "11.0.0.0/24";
    char tunnel_local[MAX_IP] = "10.0.0.61";
    char tunnel_remote[MAX_IP] = "10.0.0.228";

    // Setup the structure
    
    printf("Set up the spd node structure.\n");
    // First setup the identification variables
    strcpy(spd_node->name,name);
    spd_node->index = 13;
    spd_node->policy_dir = 2;
    spd_node->req_id = req_id;
    // To verifify the use of this values
    spd_node->ext_seq_num = false;
    spd_node->seq_overflow = false;
    spd_node->anti_replay_window = 0;

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
}


void 
test_spd() {

    printf("------------------------------------------\n");
    printf("\n\t\tSPD TEST\n");
    printf("------------------------------------------\n");
    sleep(1);
    printf("TEST 1: Add trusted spd entry.\n");

    struct spd_entry_node *spd_node = create_spd_node();
    printf("\tFirst step: create spd node data structure to send (assuming it has been already received by sysrepo)\n");
    fill_test_spd(spd_node);

    spd_entry_node *rec_spd = (spd_entry_node*) malloc(sizeof(spd_entry_node)); 
    /******************************************************************/
    sleep(2);
    printf("\tSecond step: call add_trusted_spd_entry function to send the new spd entry to the trusted part.\n");
    add_trusted_spd_entry(rec_spd,spd_node);
    printf("\tThird step: call pf_addpolicy function to install the new spd entry in the untrusted kernel through PFKEY API.\n");

    pf_addpolicy(rec_spd);
    sleep(2);
    /*****************************************************************/
    
    pf_dump_policies();
    sleep(2);
    printf("\n\nTEST 2: Verify spd node.\n");
    printf("\tFirst step: retrieve the spd entry from the untrusted kernel through pf_getpolicy function.\n");
    if(pf_getpolicy(spd_node, rec_spd) !=0) {
        ERR("An error has ocurred");
    }
    char verify_response[32];
    sleep(1);
    printf("\tSecond step: call the function verify_trusted_spd_entry.\n");
    int verification = verify_trusted_spd_entry(verify_response, rec_spd);
    switch (verification)
			{
			case 1:
				// Socket error 
				break;
			case 2:
				ERR("ALERT with %s", verify_response);
				ERR("Invalid verification of %s: \tREQID: %d",rec_spd->name,rec_spd->req_id);
				break;
			case 3:
				ERR("INVALID ANSWER!");
				break;
			default:
				printf("\tCorrect verification of %s: \t REQID: %d\n",rec_spd->name,rec_spd->req_id);
				break;
			}
    // /****************************************************************/
    
    // TODO: fix this
    sleep(2);
    printf("\n\nTEST 3: Delete trusted spd entry\n");
    printf("\tFirst step: retrieve from untrusted kernel the spd we want to delete.\n");
    printf("\tSecond step: call del_trusted_spd_entry to delete the spd from the trusted database.\n");
    del_trusted_spd_entry(rec_spd->name);
    sleep(1);
    printf("\tThird step: remove the spd from the untrusted kernel through pf_delpolicy function.\n");

    pf_delpolicy(rec_spd);
    // /****************************************************************/

    sleep(2);
    printf("Application exit requested, exiting.\n");
    exit(0);


    
    return 0;
}


