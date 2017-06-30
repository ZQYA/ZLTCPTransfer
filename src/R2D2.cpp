#include "dktool.hpp" #include <arpa/inet.h> #include <strings.h>

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "luke.hpp"
#include <fcntl.h>
#include "brightside.hpp"
int testclient(const char *host, int port) {
//	int fd = open("1.jpg",O_RDONLY);
//	char buffer[1024];
//	char *content = (char *)malloc(1024*20);
//	int read_size = 0;
//	int all_read_size = 0;
//	while((read_size = read(fd,buffer,1024))!=0) {
//		all_read_size+=read_size;	
//		if(all_read_size>1024*20) {
//			content = (char *)realloc(content,all_read_size*2);	
//		}
//		memcpy(content+all_read_size-read_size,buffer,read_size);
//		bzero(buffer,1024);
//	}
//	
	SOCKET sk_fd = prepare_send(host,port);	
	int write_size = mp_file_write(sk_fd,"1.jpg",1);
	while(1) {
		
	}
	return write_size;
}


int main() {
	testclient("127.0.0.1",8000);
}
