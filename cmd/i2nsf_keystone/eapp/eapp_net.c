#include "eapp/eapp_net.h"
#include "app/syscall.h"
#include <string.h>
#include "eapp/printf.h"

#define OCALL_CLOSE       1
#define OCALL_SEND        2
#define OCALL_RECV        3
#define OCALL_SOCKET      4

#define OCALL_NET_FREE    5
#define OCALL_NET_BIND    6
#define OCALL_NET_ACCEPT  7

#define NET_BUFFER_SIZE 16896

int custom_close(int fd) {
    int ret, retval;
    ret = ocall(OCALL_CLOSE, &fd, sizeof(int), &retval, sizeof(int));
    return ret|retval;
}

int custom_send(int sockfd, const void *buf, size_t len, int flags) {
    int ret; 
    size_t retval=0;
    unsigned  char tmp_buf[NET_BUFFER_SIZE];
    if(len > NET_BUFFER_SIZE - sizeof(net_data_t))
        return -1;
    net_data_t data_to_send;
    data_to_send.sockfd = sockfd;
    data_to_send.buf = tmp_buf + sizeof(net_data_t);
    data_to_send.len = len;
    data_to_send.flags = flags;
    memcpy(tmp_buf+sizeof(net_data_t), buf, len);
    memcpy(tmp_buf, &data_to_send, sizeof(net_data_t));
    ret = ocall(OCALL_SEND, (void*) tmp_buf, len+sizeof(net_data_t), &retval, sizeof(size_t));
    return ret|retval;
}

int custom_recv(int sockfd, const void *buf, size_t len, int flags) {
    int ret;
    unsigned char tmp_buf[NET_BUFFER_SIZE];
    if(len > NET_BUFFER_SIZE - sizeof(net_data_t))
        return -1;
    net_data_t data_to_send;
    data_to_send.sockfd = sockfd;
    data_to_send.buf = NULL;
    data_to_send.len = len;
    data_to_send.flags = flags;
    memcpy(tmp_buf, &data_to_send, sizeof(net_data_t));
    ret = ocall(OCALL_RECV, (void*) tmp_buf, sizeof(net_data_t), (void*) tmp_buf, len+sizeof(net_data_t));
    net_data_t* recv_data = ((net_data_t*) tmp_buf);
    recv_data->buf = tmp_buf+sizeof(net_data_t);
    if((!ret) && (recv_data->len > 0))
        memcpy(buf, recv_data->buf, recv_data->len);
    return ret|recv_data->len;
}

int custom_socket(int domain, int type, int protocol) {
    int ret, retval;
    unsigned char tmp_buf[sizeof(net_socket_t)];
    net_socket_t data;
    data.domain = domain;
    data.type = type;
    data.protocol = protocol;
    memcpy(tmp_buf, &data, sizeof(net_socket_t));
    ret = ocall(OCALL_SEND, (void*) tmp_buf, sizeof(net_socket_t), &retval, sizeof(int));
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

int custom_net_send(void *ctx, const unsigned char *buf, size_t len) {
    int ret, retval;
    unsigned  char tmp_buf[2048+sizeof(int)];
    if(len > 2048)
        return -1;
    int *fd = (int*) tmp_buf;
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
    // printf("ocall returned %d\n", ret);
    int retval = * ((int*)tmp_buf);
    memcpy(buf, tmp_buf+sizeof(int), len);
    // printf("Asked for %lu bytes, received %d: %s\n", len, retval, tmp_buf+sizeof(int));
    // fflush(stdout);
    return ret|retval;
}