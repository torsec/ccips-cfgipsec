#ifndef _EAPP_NET_H_
#define _EAPP_NET_H_

typedef struct {
    int sockfd;
    unsigned char *buf;
    size_t len;
    int flags;
} net_data_t;

typedef struct {
    int sockfd;
} net_sockfd_t;

typedef struct {
    int domain;
    int type;
    int protocol;
} net_socket_t;

int custom_close(int fd);
int custom_send(int sockfd, const void *buf, size_t len, int flags);
int custom_recv(int sockfd, const void *buf, size_t len, int flags);
int custom_socket(int domain, int type, int protocol);

int custom_net_connect(mbedtls_net_context *ctx, const char *host, const char *port, int proto);
void custom_net_free(mbedtls_net_context *ctx);
int custom_net_bind(mbedtls_net_context *ctx, const char *bind_ip, const char *port, int proto); 
int custom_net_accept(mbedtls_net_context *bind_ctx, mbedtls_net_context *client_ctx, void *client_ip, size_t buf_size, size_t *ip_len);

#endif