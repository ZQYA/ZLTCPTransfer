#include "dktool.hpp"
#include <arpa/inet.h>
#include <strings.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
int testclient(char *host, int port) {
	SOCKET sk_fd = dk_socket();
/// int dk_connect(SOCKET socket, const struct sockaddr *address,socklen_t address_len);
	sockaddr_in server_addr;
	server_addr.sin_addr.s_addr = inet_addr(host);
	server_addr.sin_port = htons(port);
	dk_connect(sk_fd,(const struct sockaddr *)&server_addr,sizeof(struct sockaddr_in));
	char teststr[] = "hello world";
	
	int write_size = dk_write(sk_fd,teststr,strlen(teststr));
	printf("write size:%d",write_size);	
	return write_size;
}


int main() {
	testclient("127.0.0.1",8000);
}
