#include <sys/socket.h>
#include <cstdio>
#include <unistd.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
typedef int SOCKET;
extern int tcp_backlog_size;   /// change this value can change the tcp connected and conneting buffer size
SOCKET dk_socket();
/// bind wrapper
int dk_bind(SOCKET sk_fd,int port, sockaddr_in *p_server_addr) ;
/// listen wrapper
int dk_listen(SOCKET sk_fd);
/// accept wrapper
SOCKET dk_accept(SOCKET sk_fd, sockaddr_in *p_server_addr);
/// socket read wrapper
int dk_read(SOCKET fd,void *buffer, size_t n);
/// socket write wrapper
int dk_write(SOCKET fd,void *buffer, size_t n);
/// socket connect weapper
int dk_connect(SOCKET socket, const struct sockaddr *address,socklen_t address_len);
/// start deamon process
void dk_deamonInit();
#define dk_perror(fmt,...) printf("%s:%d",__file__,__line__);perror(fmt,__VA_ARGS__);
#define dk_close(fd) close(fd)
