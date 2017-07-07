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
int heartbeat_enable = true;
void (*heartbeat_faild_handler)(void) = NULL ;
int prepare_send(const char *host, int port, void (*func)(void)) {
	SOCKET sk_fd = dk_socket();
	sockaddr_in server_addr;
	server_addr.sin_addr.s_addr = inet_addr(host);
	server_addr.sin_port = htons(port);
    server_addr.sin_family = AF_INET;
	dk_connect(sk_fd,(const struct sockaddr *)&server_addr,sizeof(struct sockaddr_in));
	pthread_t ping_th;
    heartbeat_faild_handler = func;
	int th_res = pthread_create(&ping_th,NULL,(void* (*)(void *))startping,(void*)host);
	if(0 == th_res)
		pthread_detach(ping_th);
	else 
		dk_perror("crate ping thread failed");
	return sk_fd;
}
void close_connetc(SOCKET sk_fd) {
	heartbeat_enable = false;
	close(sk_fd);
}

void startping(const char *dstIp) {
	timeval timeout;
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;
    SOCKET sk_fd = dk_socket();
    if (sk_fd < 0) {
        dk_perror("socket create failed");
        return;
    }
//    int flags = fcntl(sk_fd, F_GETFL, 0);
//    flags &= ~O_NONBLOCK;
//    fcntl(sk_fd, F_SETFL, flags);
	int conf_re = setsockopt(sk_fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
	if(-1 == conf_re) {
		dk_perror("recv conf failed");
	}	
	conf_re = setsockopt(sk_fd,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));
	if(-1 == conf_re) {
		dk_perror("send conf failed");
	}
    sockaddr_in dst;
    dst.sin_family = AF_INET;
    dst.sin_addr.s_addr = inet_addr(dstIp);
    dst.sin_port = htons(7777);
    int retryTime = 0;
    ssize_t co = dk_connect(sk_fd,(const struct sockaddr *)&dst,sizeof(struct sockaddr_in));
	while (co!=-1 && heartbeat_enable && retryTime<10) {
		usleep(1000*1000);
        const char *heartbeat = "heartbeat";
        ssize_t  wr = mp_write(sk_fd, heartbeat, strlen(heartbeat)+1, 0, true,NULL);
		if (0 >= wr) {
            ++retryTime;
			continue;
		}
        int filetype = 0;
        struct mmtp mp;
        initilizer_mmtp(&mp);
        ssize_t re =  mp_read(sk_fd, &filetype, &mp);
        if (re <= 0) {
            ++retryTime;
            dk_perror("back failed");
        }
        struct mmtp *pmp = &mp;
        destory_mmtp(&pmp);
	}
    if (heartbeat_enable&&(co==-1||retryTime!=0) && heartbeat_faild_handler!=NULL) {
        heartbeat_faild_handler();
    }
}
