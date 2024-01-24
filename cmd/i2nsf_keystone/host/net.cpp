#include "edge/edge_call.h"
#include "host/keystone.h"
#include "net.h"
#include <stdio.h>

#define NET_BUFFER_SIZE 16896

void
net_send_wrapper(void* buffer) {
  /* Parse and validate the incoming call data */
  struct edge_call* edge_call = (struct edge_call*)buffer;
  uintptr_t call_args;
  size_t arg_len;
  size_t ret_val;
  size_t ret_len;
  if (edge_call_args_ptr(edge_call, &call_args, &arg_len) != 0) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }

  net_data_t* data_to_send = (net_data_t *) call_args;
  data_to_send->buf = (unsigned char *) call_args + sizeof(net_data_t);
  /* Pass the arguments from the eapp to the exported ocall function */
  ret_val = write(data_to_send->fd, data_to_send->buf, data_to_send->len);

  /* Setup return data from the ocall function */
  uintptr_t data_section = edge_call_data_ptr();
  ret_len = sizeof(size_t);
  memcpy((void*)data_section, &ret_val, ret_len);
  if (edge_call_setup_ret(
          edge_call, (void*)data_section, ret_len)) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
  } else {
    edge_call->return_data.call_status = CALL_STATUS_OK;
  }

  /* This will now eventually return control to the enclave */
  return;
}

void
net_recv_wrapper(void* buffer) {
  /* Parse and validate the incoming call data */
  struct edge_call* edge_call = (struct edge_call*)buffer;
  uintptr_t call_args;
  size_t arg_len;
  size_t ret_val;
  size_t ret_len;
  if (edge_call_args_ptr(edge_call, &call_args, &arg_len) != 0) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }

  unsigned char recv_buffer[NET_BUFFER_SIZE+sizeof(int)] = {0};
  net_data_t* data_to_recv = (net_data_t*) call_args;

  if(data_to_recv->len > NET_BUFFER_SIZE)
    ret_val = -1;
  else {
    ret_val = read(data_to_recv->fd, recv_buffer, data_to_recv->len);
  }

  /* Setup return data from the ocall function */
  uintptr_t data_section = edge_call_data_ptr();
  net_data_t ret_data;

  if(ret_val > 0) {
    memcpy((void*)data_section+sizeof(net_data_t), recv_buffer, ret_val);
    ret_data.buf = (unsigned char *) data_section+sizeof(net_data_t);
    ret_data.len = ret_val;
    ret_len = sizeof(net_data_t) + ret_val;
  } else {
    ret_data.buf = NULL;
    ret_data.len = ret_val;
    ret_len = sizeof(net_data_t);
  }
  
  memcpy((void*)data_section, &ret_data, sizeof(net_data_t));
  if (edge_call_setup_ret(
          edge_call, (void*)data_section, ret_len)) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
  } else {
    edge_call->return_data.call_status = CALL_STATUS_OK;
  }

  /* This will now eventually return control to the enclave */
  return;
}

void
net_free_wrapper(void* buffer) {
  /* Parse and validate the incoming call data */
  struct edge_call* edge_call = (struct edge_call*)buffer;
  uintptr_t call_args;
  unsigned long ret_val;
  size_t arg_len;
  if (edge_call_args_ptr(edge_call, &call_args, &arg_len) != 0) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }

  mbedtls_net_context server_fd;
  server_fd.fd = *((int*)call_args);
  mbedtls_net_free(&server_fd);
  ret_val = 0;

  /* Setup return data from the ocall function */
  uintptr_t data_section = edge_call_data_ptr();
  memcpy((void*)data_section, &ret_val, 0);
  if (edge_call_setup_ret(
          edge_call, (void*)data_section, 0)) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
  } else {
    edge_call->return_data.call_status = CALL_STATUS_OK;
  }

  /* This will now eventually return control to the enclave */
  return;
}

void
net_connect_wrapper(void* buffer) {
  /* Parse and validate the incoming call data */
  struct edge_call* edge_call = (struct edge_call*)buffer;
  uintptr_t call_args;
  int ret_val;
  size_t arg_len;
  if (edge_call_args_ptr(edge_call, &call_args, &arg_len) != 0) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }

  mbedtls_net_context server_fd;
  mbedtls_net_init(&server_fd);
  /* Pass the arguments from the eapp to the exported ocall function */
  ret_val = mbedtls_net_connect(&server_fd, SERVER_NAME, (char*) call_args, MBEDTLS_NET_PROTO_TCP);

  net_connect_t ret;
  ret.fd = server_fd.fd;
  ret.retval = ret_val;

  /* Setup return data from the ocall function */
  uintptr_t data_section = edge_call_data_ptr();
  memcpy((void*)data_section, &ret, sizeof(net_connect_t));
  if (edge_call_setup_ret(
          edge_call, (void*)data_section, sizeof(net_connect_t))) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
  } else {
    edge_call->return_data.call_status = CALL_STATUS_OK;
  }

  /* This will now eventually return control to the enclave */
  return;
}

void
net_bind_wrapper(void* buffer) {
  /* Parse and validate the incoming call data */
  struct edge_call* edge_call = (struct edge_call*)buffer;
  uintptr_t call_args;
  int ret_val;
  size_t arg_len;
  if (edge_call_args_ptr(edge_call, &call_args, &arg_len) != 0) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }

  mbedtls_net_context server_fd;
  mbedtls_net_init(&server_fd);
  /* Pass the arguments from the eapp to the exported ocall function */
  ret_val = mbedtls_net_bind(&server_fd, NULL, (char*) call_args, MBEDTLS_NET_PROTO_TCP);

  net_connect_t ret;
  ret.fd = server_fd.fd;
  ret.retval = ret_val;

  /* Setup return data from the ocall function */
  uintptr_t data_section = edge_call_data_ptr();
  memcpy((void*)data_section, &ret, sizeof(net_connect_t));
  if (edge_call_setup_ret(
          edge_call, (void*)data_section, sizeof(net_connect_t))) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
  } else {
    edge_call->return_data.call_status = CALL_STATUS_OK;
  }

  /* This will now eventually return control to the enclave */
  return;
}

void
net_accept_wrapper(void* buffer) {
  /* Parse and validate the incoming call data */
  struct edge_call* edge_call = (struct edge_call*)buffer;
  uintptr_t call_args;
  int ret_val;
  size_t arg_len;
  if (edge_call_args_ptr(edge_call, &call_args, &arg_len) != 0) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }

  mbedtls_net_context client_fd;
  mbedtls_net_context server_fd;
  mbedtls_net_init(&client_fd);
  server_fd.fd = *((int*)call_args);

  /* Pass the arguments from the eapp to the exported ocall function */
  ret_val = mbedtls_net_accept(&server_fd, &client_fd, NULL, 0, NULL);

  net_connect_t ret;
  ret.fd = client_fd.fd;
  ret.retval = ret_val;

  /* Setup return data from the ocall function */
  uintptr_t data_section = edge_call_data_ptr();
  memcpy((void*)data_section, &ret, sizeof(net_connect_t));
  if (edge_call_setup_ret(
          edge_call, (void*)data_section, sizeof(net_connect_t))) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
  } else {
    edge_call->return_data.call_status = CALL_STATUS_OK;
  }

  /* This will now eventually return control to the enclave */
  return;
}
