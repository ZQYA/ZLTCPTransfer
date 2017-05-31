#include <sys/socket.h>
#include <cstdio>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
typedef int SOCKET;
SOCKET dk_socket();

int dk_bind(SOCKET sk_fd,int port, sockaddr_in *p_server_addr) ;

int dk_listen(SOCKET sk_fd);

int dk_accept(SOCKET sk_fd, sockaddr_in *p_server_addr);
