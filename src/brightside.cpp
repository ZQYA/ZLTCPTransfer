#include "brightside.hpp"
#include "dktool.hpp"
#include <arpa/inet.h>
#include <strings.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "luke.hpp"
#include <fcntl.h>
#include <unistd.h>
#include "icmp_protocol.hpp"
extern int heart_beat_enable ; ///heart beat run as the client's life cricle. if close the client connect, close the heart;
int prepare_send(const char *host, int port) {
	SOCKET sk_fd = dk_socket();
	sockaddr_in server_addr;
	server_addr.sin_addr.s_addr = inet_addr(host);
	server_addr.sin_port = htons(port);
    server_addr.sin_family = AF_INET;
	dk_connect(sk_fd,(const struct sockaddr *)&server_addr,sizeof(struct sockaddr_in));
	return sk_fd;
}

void startping(const char *dstIp) {
	timeval timeout;
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;
	SOCKET sk_fd = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);
	int conf_re = setsockopt(sk_fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
	if(-1 == conf_re) {
		perror("recv conf failed");
	}	
	conf_re = setsockopt(sk_fd,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));
	if(-1 == conf_re) {
		perror("send conf failed");
	}	
	if (sk_fd < 0) {
		perror("socket create failed");
		return;
	}
	sockaddr_in dst;
	dst.sin_family = AF_INET;
	dst.sin_addr.s_addr = inet_addr(dstIp);
	while (true && heart_beat_enable) {
		usleep(1000*1000);
		int re = sendEchoRequest(sk_fd,dst);	
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
