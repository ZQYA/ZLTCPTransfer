#include "icmp_protocol.hpp"
#include <sys/time.h>
#include <errno.h>
long GetTickCount() {
	struct timeval time;
	int res = gettimeofday(&time,NULL);
	if(0 == res) {
		return time.tv_sec;	
	}else {
		return 0;
	}
	
}
ssize_t sendEchoRequest(SOCKET sock , sockaddr_in dstIP) {
	 static int id = 1;
	 static int seq = 1;
		     
	//ICMP请求
	ECHOREQUEST echoRequest ;
 	//主要是用来记录请求应答的时间，当发送ECHO请求时记录发送时间，当接受到应答数据时，在用GetTickcount()- echoRequest.time这样就能得到请求应答需要多少时间了。
	echoRequest.time = GetTickCount();   
	echoRequest.icmpHeader.type = 8;
	echoRequest.icmpHeader.code = 0;
	echoRequest.icmpHeader.id = id++;
	echoRequest.icmpHeader.seq = seq++;
		
	ssize_t re = sendto(sock,(char *)&echoRequest,sizeof(echoRequest), 0, (sockaddr*)&dstIP, sizeof(dstIP));
	
	if (-1 == re)
	{
		perror(" send error \n ");
	}
	return re;
}

ssize_t recvEchoReQuest(SOCKET sock,ECHORESPONSE * sponse, sockaddr_in *dstIP) {
	socklen_t size = sizeof(sockaddr_in);
	ssize_t  re = recvfrom(sock, (char *)sponse, sizeof(ECHORESPONSE), 0,(struct sockaddr*)dstIP, &size);
	if(-1 == re)
	{
		perror("recvfrom error");
	}
	return re;
}
