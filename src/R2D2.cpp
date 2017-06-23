#include "dktool.hpp"
#include <arpa/inet.h>
#include <strings.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "luke.hpp"
int testclient(const char *host, int port) {
	SOCKET sk_fd = dk_socket();
	sockaddr_in server_addr;
	server_addr.sin_addr.s_addr = inet_addr(host);
	server_addr.sin_port = htons(port);
	dk_connect(sk_fd,(const struct sockaddr *)&server_addr,sizeof(struct sockaddr_in));
	char teststr[] = "hello world";
	int write_size = mp_write(sk_fd,teststr,strlen(teststr),0,true);
	dk_close(sk_fd);
	return write_size;
}


int main() {
	testclient("127.0.0.1",8000);
}
