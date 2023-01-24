#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
// #ifdef __wasi__
// #include <wasi_socket_ext.h>
// #endif


// #include <emscripten.h>
// https://github.com/bytecodealliance/wasm-micro-runtime/tree/main/samples/socket-ap

#define PORT 4444
#define BACKLOG 5


int main(int argc, char **argv) {
  int sockfd, new_fd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t sin_size;

  printf("%s\n","test");

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      perror("socket");
      exit(1);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  // bzero(&(server_addr.sin_zero), 8);

  if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
      perror("bind");
      exit(1);
  }

  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }

  sin_size = sizeof(struct sockaddr_in);
if ((new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size)) == -1) {
    perror("accept");
    exit(1);
}

char *message = "Hello, client!\n";
send(new_fd, message, strlen(message), 0);


close(new_fd);
close(sockfd);
return 0;
}