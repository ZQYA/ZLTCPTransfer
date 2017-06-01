#include <sys/socket.h>
#include <cstdio>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
typedef int SOCKET;
SOCKET dk_socket();
/// bind wrapper
int dk_bind(SOCKET sk_fd,int port, sockaddr_in *p_server_addr) ;
/// listen wrapper
int dk_listen(SOCKET sk_fd);
/// accept wrapper
int dk_accept(SOCKET sk_fd, sockaddr_in *p_server_addr);
/// socket read wrapper
int dk_read(SOCKET fd,void *buffer, size_t n);
/// socket write wrapper
int dk_write(SOCKET fd,void *buffer, size_t n);
