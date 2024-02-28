#ifndef __PFKEYV2_UTILS
#define __PFKEYV2_UTILS

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/pfkeyv2.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "utils.h"


void print_sadb_msg(struct sadb_msg *msg, int msglen);
void sa_print(struct sadb_ext *ext);
void supported_print(struct sadb_ext *ext);
void lifetime_print(struct sadb_ext *ext);
void address_print(struct sadb_ext *ext);
void key_print(struct sadb_ext *ext);

#ifdef __cplusplus
}
#endif

#endif