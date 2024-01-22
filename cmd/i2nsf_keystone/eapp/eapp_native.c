//******************************************************************************
// Copyright (c) 2018, The Regents of the University of California (Regents).
// All Rights Reserved. See LICENSE for license details.
//------------------------------------------------------------------------------
#include "eapp_utils.h"
#include "string.h"
#include "edge_call.h"
#include <syscall.h>
#include "printf.h"
#include "malloc.h"

extern char* __malloc_start;
extern char* __malloc_zone_stop;

#define OCALL_PRINT_STRING 1
// #define PUBLIC_KEY_SIZE         32
// #define PRIVATE_KEY_SIZE        64
// #define KEYSTONE_PK_SIGNATURE_MAX_SIZE 64
// #define CERTS_MAX_LEN           1024

#define HEAP_SIZE ((char *)(&__malloc_zone_stop) - (char *)(&__malloc_start))

unsigned long ocall_print_string(char* string);

int main(){
  /*
  unsigned char pk[PUBLIC_KEY_SIZE] = {0};
  unsigned char ldevid_crt[CERTS_MAX_LEN] = {0};
  int ldevid_crt_len = 0;
  unsigned char* certs[3];
  int sizes[3];
  unsigned char seed[PRIVATE_KEY_SIZE] = { 
    0x30, 0x82, 0x05, 0x32, 0x30, 0x82, 0x04, 0xe2, 0x02, 0x01, 0x00, 0x30, 0x45, 0x31, 0x0c, 0x30, 
    0x0a, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x03, 0x42, 0x6f, 0x62, 0x31, 0x35, 0x30, 0x33, 0x06, 
    0x03, 0x55, 0x04, 0x0a, 0x0c, 0x2c, 0x45, 0x6e, 0x63, 0x6c, 0x61, 0x76, 0x65, 0x2d, 0x62, 0x62, 
    0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x2d, 0x62, 0x62, 0x62, 0x62, 0x2d, 0x62, 0x62, 0x62, 0x62 
  };
  unsigned char signature[KEYSTONE_PK_SIGNATURE_MAX_SIZE];
  size_t sig_len;
  */

  umm_init_heap(&__malloc_start, HEAP_SIZE);

  /*
  certs[0] = calloc(1, CERTS_MAX_LEN);
  if(certs[0]==NULL){
      EAPP_RETURN(-1);
  }
  certs[1] = calloc(1, CERTS_MAX_LEN);
  if(certs[1]==NULL){
      free(certs[0]);
      EAPP_RETURN(-1);
  }
  certs[2] = calloc(1, CERTS_MAX_LEN);
  if(certs[2]==NULL){
      free(certs[0]);
      free(certs[1]);
      EAPP_RETURN(-1);
  }
  */

  ocall_print_string("Hello World");
  // custom_printf("Test\n");

  /*
  create_keypair(pk, 15, ldevid_crt, &ldevid_crt_len);
  custom_printf("PK: ");
  print_hex_string(pk, PUBLIC_KEY_SIZE);
  custom_printf("\ncrt: ");
  print_hex_string(ldevid_crt, ldevid_crt_len);
  custom_printf("\ncrt_len: %d\n", ldevid_crt_len);
  
  get_cert_chain(certs[0], certs[1], certs[2], &sizes[0], &sizes[1], &sizes[2]);
  custom_printf("crt_0: ");
  print_hex_string(certs[0], sizes[0]);
  custom_printf("\ncrt_0_len: %d\n", sizes[0]);
  custom_printf("crt_1: ");
  print_hex_string(certs[1], sizes[1]);
  custom_printf("\ncrt_1_len: %d\n", sizes[1]);
  custom_printf("crt_2: ");
  print_hex_string(certs[2], sizes[2]);
  custom_printf("\ncrt_2_len: %d\n", sizes[2]);
  
  crypto_interface(2, seed, PRIVATE_KEY_SIZE, signature, &sig_len, pk);
  custom_printf("\nsig: ");
  print_hex_string(signature, sig_len);
  custom_printf("\nsig_len: %d\n", sig_len);

  free(certs[0]);
  free(certs[1]);
  free(certs[2]);
  */

  EAPP_RETURN(0);
}

unsigned long ocall_print_string(char* string){
  unsigned long retval;
  ocall(OCALL_PRINT_STRING, string, strlen(string)+1, &retval ,sizeof(unsigned long));
  return retval;
}
