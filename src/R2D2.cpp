#include "dktool.hpp"
#include <arpa/inet.h>
#include <strings.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "luke.hpp"
#include <fcntl.h>
int testclient(const char *host, int port) {
	SOCKET sk_fd = dk_socket();
	sockaddr_in server_addr;
	server_addr.sin_addr.s_addr = inet_addr(host);
	server_addr.sin_port = htons(port);
	dk_connect(sk_fd,(const struct sockaddr *)&server_addr,sizeof(struct sockaddr_in));
	int fd = open("1.jpg",O_RDONLY);
	char buffer[1024];
	char *content = (char *)malloc(1024*20);
	int read_size = 0;
	int all_read_size = 0;
	while((read_size = read(fd,buffer,1024))!=0) {
		all_read_size+=read_size;	
		if(all_read_size>1024*20) {
			content = (char *)realloc(content,all_read_size*2);	
		}
		memcpy(content+all_read_size-read_size,buffer,read_size);
		bzero(buffer,1024);
	}
	int write_size = mp_write(sk_fd,content,all_read_size,1,true);
	dk_close(sk_fd);
	return write_size;
}


int main() {
	testclient("127.0.0.1",8000);
}
