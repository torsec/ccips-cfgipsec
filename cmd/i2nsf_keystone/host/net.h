#ifndef _HOST_NET_H_
#define _HOST_NET_H_
#include <sys/socket.h>

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

typedef struct {
    int sockfd;
    struct sockaddr *addr;
    socklen_t addrlen;
} net_connect_t;

void
close_wrapper(void* buffer);
void
send_wrapper(void* buffer);
void
recv_wrapper(void* buffer);
void
socket_wrapper(void* buffer);

void
net_connect_wrapper(void* buffer);
void
net_free_wrapper(void* buffer);
void
net_bind_wrapper(void* buffer);
void 
net_accept_wrapper(void* buffer);

#endif
