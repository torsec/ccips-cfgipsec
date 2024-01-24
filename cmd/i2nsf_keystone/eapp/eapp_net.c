#include "eapp/eapp_net.h"
#include "app/syscall.h"
#include <string.h>
#include "eapp/printf.h"

#define OCALL_NET_CONNECT 1
#define OCALL_NET_SEND    2
#define OCALL_NET_RECV    3
#define OCALL_NET_FREE    4
#define OCALL_NET_BIND    5
#define OCALL_NET_ACCEPT  6

#define NET_BUFFER_SIZE 16896

int custom_net_send(int fd, const unsigned char *buf, size_t len) {
    int ret, retval;
    unsigned  char tmp_buf[NET_BUFFER_SIZE];
    if(len > NET_BUFFER_SIZE - sizeof(net_data_t))
        return -1;
    net_data_t data_to_send;
    data_to_send
    *fd = ((mbedtls_net_context *) ctx)->fd;
    memcpy(tmp_buf+sizeof(int), buf, len);
    ret = ocall(OCALL_NET_SEND, (unsigned char *)tmp_buf, len+sizeof(int), &retval, sizeof(int));
    return ret|retval;
}

int custom_net_recv(void *ctx, unsigned char *buf, size_t len) {
    int ret;
    unsigned char tmp_buf[16896+sizeof(int)];
    int *fd = (int*) tmp_buf;
    *fd = ((mbedtls_net_context *) ctx)->fd;
    ret = ocall(OCALL_NET_RECV, tmp_buf, len, tmp_buf, len + sizeof(int));
    #if PERFORMANCE_TEST
    custom_printf("\n[OCALL_NET_RECV] ...Receiving\n");
    t_end = get_time_inline();
    t_diff = t_end - t_start;
    custom_printf("\nTicks between request and response: %lu\n", t_diff);
    #endif
    // custom_printf("ocall returned %d\n", ret);
    int retval = * ((int*)tmp_buf);
    memcpy(buf, tmp_buf+sizeof(int), len);
    // custom_printf("Asked for %lu bytes, received %d: %s\n", len, retval, tmp_buf+sizeof(int));
    return ret|retval;
}

void custom_net_free(mbedtls_net_context *ctx) {
    int fd = ((mbedtls_net_context *) ctx)->fd;
    ocall(OCALL_NET_FREE, (unsigned char *) &fd, sizeof(int), NULL, 0);
}

int custom_net_connect(mbedtls_net_context *ctx, const char *host, const char *port, int proto) {
    int ret;
    net_connect_t retval;
    char tmp[16] = {0};
    memcpy(tmp, port, 5);
    #if PERFORMANCE_TEST
    t_start = get_time_inline();
    #endif
    ret = ocall(OCALL_NET_CONNECT, tmp, 5,(void*) &retval, sizeof(net_connect_t));
    #if PERFORMANCE_TEST
    t_end = get_time_inline();
    t_diff = t_end - t_start;
    custom_printf("\n[OCALL_NET_CONNECT] Ticks: %lu\n", t_diff);
    #endif
    ret |= retval.retval;
    // custom_printf("net_connect - fd: %d, ret: %d\n", retval.fd, retval.retval);
    if(ret) {
        return ret;
    } else {
        ctx->fd = retval.fd;
    }
    return 0;
}

int custom_net_bind(mbedtls_net_context *ctx, const char *bind_ip, const char *port, int proto) {
    int ret;
    net_connect_t retval;
    char tmp[16] = {0};
    memcpy(tmp, port, 5);
    ret = ocall(OCALL_NET_BIND, tmp, 5,(void*) &retval, sizeof(net_connect_t));
    ret |= retval.retval;
    // custom_printf("net_connect - fd: %d, ret: %d\n", retval.fd, retval.retval);
    if(ret) {
        return ret;
    } else {
        ctx->fd = retval.fd;
    }
    return 0;
}

int custom_net_accept(mbedtls_net_context *bind_ctx, mbedtls_net_context *client_ctx, void *client_ip, size_t buf_size, size_t *ip_len) {
    int fd = ((mbedtls_net_context *) bind_ctx)->fd;
    int ret;
    net_connect_t retval;
    #if PERFORMANCE_TEST
    t_start = get_time_inline();
    #endif
    ret = ocall(OCALL_NET_ACCEPT, (unsigned char *) &fd, sizeof(int), (void*) &retval, sizeof(net_connect_t));
    #if PERFORMANCE_TEST
    t_end = get_time_inline();
    t_diff = t_end - t_start;
    custom_printf("\n[OCALL_NET_ACCEPT] Ticks: %lu\n", t_diff);
    #endif
    ret |= retval.retval;
    // custom_printf("net_connect - fd: %d, ret: %d\n", retval.fd, retval.retval);
    if(ret) {
        return ret;
    } else {
        client_ctx->fd = retval.fd;
    }
    return 0;
}