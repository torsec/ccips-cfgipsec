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

int exit_application = 0;
void print_hash(char *p){
    for(unsigned int i = 0; i < 16; ++i){
        printf("%02x", p[i]);
    }
    printf("\n");
}
// sudo gcc -pthread -g -w -I /usr/include/libnl3/ -o test test.c base/* parson/parson.h parson/parson.c
static void
sigint_handler(int signum)
{
    (void)signum;

    exit_application = 1;
}

static void 
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

static void
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
    spd_node->ipsec_mode = IPSEC_MODE_TRANSPORT;
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

void *
custom_malloc(size_t size) {
    void *p;
    p = malloc(size);
    printf("Alloc POINTER = %p\n", p);
    return p;
}

void
custom_free(void *p) {
    printf("Free POINTER = %p\n", p);
    free(p);
    return;
}

int 
main(int argc, char **argv) {

    // json_set_allocation_functions(custom_malloc, custom_free);
    // TODO add test with spd 
    if ( geteuid() != 0 ) {
            fprintf ( stderr, "Must be root in order to execute cfgipsec2. You are UID=%u, EUID=%u\n", getuid(), geteuid() );
            return 1;
        }

        // Get options
        int foreground = false;
        int c;
        int l = CI_VERB_INFO;
        log_set_level(l);
        while ( ( c = getopt ( argc, argv, "f:c:v:h" ) ) != -1 ) {
            switch ( c ) {
                case 'f':
                    foreground = true; // TBD
                    break;
                case 'v':
                    l = atoi(optarg);  // Convert optarg to an integer
                    if (l < 0 || l > CI_VERB_TRACE) {
                        printf("verbose level out of range: %d\n", l);
                        exit(EXIT_FAILURE);
                    } else {
                        log_set_level(l);  // Set the log level based on the converted value
                    }
                    break;
                case 'h': {
                    fprintf(stderr, "cfgipsec2 version %s \n", VERSION);
                    fprintf(stderr, "Usage:\n" );
                    fprintf(stderr, "       %s [-v verbose_level]\n",argv[0]);
                    fprintf(stderr, "\n" );
                    fprintf(stderr, "Where:\n" );
                    fprintf(stderr, "       - case is `case1` (IKE case) or `case2` (IKE-less case, default)\n" );
                    fprintf(stderr, "       - verbose_level is 0: FATAL, 1: ERR, 2: WARN, 3: INFO (default), 4: DEBUG, 5: TRACE\n" );
                    fprintf(stderr, "" );
                    return 0;
                }
                default: {
                    fprintf(stderr, "Usage: %s [-v verbose_level]\n", argv[0]);
                    exit(EXIT_FAILURE);
                }
            }
        }

    INFO("LOG level set to: %d",l);


#ifdef Enarx
    INFO("Enarx CCIPs version");
    // Enable connectivity with enarx client
    if(connect_ta() != 0) {
        ERR("Couldnt connect to TA");
        exit(1);
    }
#endif
    connect_ta();

    
    int rc;


    printf("\n\nSAD TEST.\n\n");
    struct sad_entry_node *sad_node = create_sad_node();
    sad_entry_node *rec_sad = (sad_entry_node *)malloc(sizeof(sad_entry_node));
    fill_test_sad(sad_node);
    printf("INPUT SAD NODE before add_trusted_sad_entry:\n");
    printf("local subnet = %s\n", sad_node->local_subnet);
    printf("encryption key = %s\n", sad_node->encryption_key);
    add_trusted_sad_entry(rec_sad, sad_node);
    // after add_trusted_sad_entry() is called, the "encryption key" field of the sad is empty
    printf("INPUT SAD NODE after add_trusted_sad_entry:\n");
    printf("local subnet = %s\n", sad_node->local_subnet);
    printf("encryption key = %s\n", sad_node->encryption_key);
    printf("integrity key = %s\n", sad_node->integrity_key);
    printf("ipsec_mode = %d\n", sad_node->ipsec_mode);
    printf("\nAdding trusted sad entry...\n");
    pf_addsad(sad_node);
    
    printf("INPUT SAD NODE after pf_addsad:\n");
    printf("local subnet = %s\n", sad_node->local_subnet);
    printf("encryption key = %s\n", sad_node->encryption_key);
    pf_dump_sads(sad_node);
    // sad_entry_node *sad_out_node = create_sad_node();
    if(pf_getsad(sad_node, rec_sad) !=0) {
        ERR("An error has ocurred");
    }
    printf("OUTPUT SAD NODE (after pf_getsad):\n");
    printf("local subnet = %s\n", rec_sad->local_subnet);
    printf("encryption key = %s\n", rec_sad->encryption_key);

    printf("Verify sad node.\n");
    // verify_sad_nodes(); // this function needs sysrepo connection
    char verify_response[32];
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
				INFO("Correct verification of %s: SPI %d\t REQID: %d",rec_sad->name,rec_sad->spi,rec_sad->req_id);
				break;
			}


    printf("Delete trusted sad entry\n");
    del_trusted_sad_entry(rec_sad->name);
    
    pf_delsad(rec_sad);
    


    // printf("SPD TEST.\n");

    // struct spd_entry_node *spd_node = create_spd_node();
    // fill_test_spd(spd_node);

    // spd_entry_node *rec_spd = (spd_entry_node*) malloc(sizeof(spd_entry_node)); 
    // /******************************************************************/
    // printf("\nAdding trusted spd entry...\n");
    
    // add_trusted_spd_entry(rec_spd,spd_node);
    
    // pf_addpolicy(spd_node);

    // /*****************************************************************/
    // printf("\nDump policies.\n");
    
    // pf_dump_policies();
    // // printf("Verify policy nodes.\n");
    // // verify_spd_nodes();

    // // /****************************************************************/
    
    // // TODO: fix this
    // // if(pf_getpolicy(spd_node, rec_spd) !=0) {
    // //     ERR("An error has ocurred");
    // // }
    
    // printf("Delete trusted spd entry\n");
    // del_trusted_spd_entry(rec_spd->name);
    
    // pf_delpolicy(rec_spd);
    // /****************************************************************/

    
    printf("Application exit requested, exiting.\n");
    exit(0);


    
    return 0;
}


