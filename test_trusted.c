
#include "log.h"
#include <assert.h>
#include <stdbool.h>
#include "parson.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "sysrepo_entries.h"
#include "test_sad_spd/test_sad.h"
#include "test_sad_spd/test_spd.h"
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
    // test_sad();
    test_spd();
    printf("Application exit requested, exiting.\n");
    exit(0);


    
    return 0;
}


