#include <arpa/inet.h>
#include <strings.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include "icmp_protocol.hpp"
#include "luke.hpp"
#include "brightside.hpp"
#include "dktool.hpp"
#include <pthread/pthread.h>
int heart_beat_enable = true; 

int prepare_send(const char *host, int port) {
	SOCKET sk_fd = dk_socket();
	sockaddr_in server_addr;
	server_addr.sin_addr.s_addr = inet_addr(host);
	server_addr.sin_port = htons(port);
    server_addr.sin_family = AF_INET;
	dk_connect(sk_fd,(const struct sockaddr *)&server_addr,sizeof(struct sockaddr_in));
	pthread_t ping_th;
	int th_res = pthread_create(&ping_th,NULL,(void* (*)(void *))startping,(void*)host);
	if(0 == th_res)
		pthread_detach(ping_th);
	else 
		perror("crate ping thread failed");
	return sk_fd;
}
void close_connetc(SOCKET sk_fd) {
	heart_beat_enable = false;
	close(sk_fd);
}

void startping(const char *dstIp) {
	timeval timeout;
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;
	SOCKET sk_fd = socket(AF_INET,SOCK_DGRAM,IPPROTO_ICMP);
    if (sk_fd < 0) {
        perror("socket create failed");
        return;
    }
//    int flags = fcntl(sk_fd, F_GETFL, 0);
//    flags &= ~O_NONBLOCK;
//    fcntl(sk_fd, F_SETFL, flags);
	int conf_re = setsockopt(sk_fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
	if(-1 == conf_re) {
		perror("recv conf failed");
	}	
	conf_re = setsockopt(sk_fd,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));
	if(-1 == conf_re) {
		perror("send conf failed");
	}
	while (true && heart_beat_enable) {
		usleep(1000*1000);
		sockaddr_in dst;
		dst.sin_family = AF_INET;
		dst.sin_addr.s_addr = inet_addr(dstIp);
		ssize_t re = sendEchoRequest(sk_fd,dst);
		if (0 >= re) {
			break;
		}
		ECHORESPONSE sponse;
		re = recvEchoReQuest(sk_fd,&sponse,&dst);
		if (0 >= re) {
			break;
		}
		printf("from %s : size =32 time =%ds TTL=%d \n",inet_ntoa(dst.sin_addr),sponse.echoRequest.time,sponse.ipHeader.ttl);
	}
}
