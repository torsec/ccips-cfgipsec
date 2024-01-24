#ifndef _EAPP_NET_H_
#define _EAPP_NET_H_

typedef struct {
    int fd;
    unsigned char *buf;
    size_t len;
} net_data_t;

typedef struct {
    int fd;
} net_connect_t;

int custom_net_send(void *ctx, const unsigned char *buf, size_t len);
int custom_net_recv(void *ctx, unsigned char *buf, size_t len);

int custom_net_connect(mbedtls_net_context *ctx, const char *host, const char *port, int proto);
void custom_net_free(mbedtls_net_context *ctx);
int custom_net_bind(mbedtls_net_context *ctx, const char *bind_ip, const char *port, int proto); 
int custom_net_accept(mbedtls_net_context *bind_ctx, mbedtls_net_context *client_ctx, void *client_ip, size_t buf_size, size_t *ip_len);

#endif