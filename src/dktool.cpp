#include "dktool.hpp"
#include <cerrno>
#include <unistd.h>
int tcp_backlog_size =  100; /// default listen size
SOCKET dk_socket() {
	SOCKET sk_fd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == sk_fd) {
	 	perror("socket build faild");
		return -1;
	}
	return sk_fd;
}

int dk_bind(SOCKET sk_fd,int port, sockaddr_in *p_server_addr) {
	sockaddr_in  server_addr = *p_server_addr;
	bzero(&server_addr,sizeof(sockaddr_in));		
	server_addr.sin_family = AF_INET;
	
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	server_addr.sin_port = 	htons(port);
	int bind_stat = bind(sk_fd,(sockaddr *)&server_addr,sizeof(sockaddr_in));
	if(-1 == bind_stat)	{
		perror("bind failed");
		return -1;
	}
	return 1;
}

int dk_listen(SOCKET sk_fd) {
	int listen_stat = listen(sk_fd,6);
	if(-1 == listen_stat) {
		perror("listen failed");
		return -1;
	}
	return listen_stat;
}

SOCKET dk_accept(SOCKET sk_fd, sockaddr_in *p_server_addr) {
	socklen_t size = sizeof(sockaddr_in);
	int accept_stat = accept(sk_fd,(sockaddr*)p_server_addr,&size);
	if(-1 == accept_stat) {
		perror("accept failed");
		return -1;
	}
	return accept_stat;
}

int dk_read(SOCKET fd,void *buffer, size_t n) {		
	char *tmp = (char *)buffer;
	int read_last = n;
	int read_cout = 0;
	while (read_last > 0) {
		read_cout = read(fd,tmp,read_last);
		if (read_cout < 0 ) {
			if (EINTR==errno)
				read_cout = 0;
			else 	
				break;
		} 
		read_last -= read_cout;
		tmp += read_cout;
	}
	return n-read_last; 
}

int dk_write(SOCKET fd,void *buffer, size_t n) {
	char *tmp = (char *)buffer;
	int write_last = n;
	int write_count = 0;
	while(write_last > 0) {
		write_count = write(fd,tmp,write_last);
		if(write_count < 0) {
			if (EINTR == errno)
				write_count = 0;
			else 
				break;
		}
		write_last -= write_count;
		tmp += write_count;
	}
	return n - write_last;
}
