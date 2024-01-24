#ifndef _HOST_NET_H_
#define _HOST_NET_H_

typedef struct {
    int fd;
    unsigned char *buf;
    size_t len;
} net_data_t;

typedef struct {
    int fd;
} net_connect_t;

void
net_send_wrapper(void* buffer);
void
net_recv_wrapper(void* buffer);

void
net_connect_wrapper(void* buffer);
void
net_free_wrapper(void* buffer);
void
net_bind_wrapper(void* buffer);
void 
net_accept_wrapper(void* buffer);

#endif
