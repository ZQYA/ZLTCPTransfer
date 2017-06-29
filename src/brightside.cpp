#include "brightside.hpp"
#include "dktool.hpp"
#include <arpa/inet.h>
#include <strings.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "luke.hpp"
#include <fcntl.h>
int prepare_send(const char *host, int port) {
	SOCKET sk_fd = dk_socket();
	sockaddr_in server_addr;
	server_addr.sin_addr.s_addr = inet_addr(host);
	server_addr.sin_port = htons(port);
    server_addr.sin_family = AF_INET;
	dk_connect(sk_fd,(const struct sockaddr *)&server_addr,sizeof(struct sockaddr_in));
	return sk_fd;
}
