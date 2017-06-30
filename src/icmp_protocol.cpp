#include "icmp_protocol.hpp"
#include <sys/time.h>
int GetTickCount() {
	struct timeval time;
	int res = gettimeofday(&time,NULL);
	if(0 == res) {
		return time.tv_sec;	
	}else {
		return 0;
	}
	
}
int sendEchoRequest(SOCKET sock , sockaddr_in dstIP) {
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
	echoRequest.icmpHeader.ck_sum= checksum((u_short*)&echoRequest, sizeof(echoRequest));
		
	int re = sendto(sock,(char *)&echoRequest,sizeof(echoRequest), 0, (sockaddr*)&dstIP, sizeof(dstIP));
	
	if (-1 == re)
	{
		perror(" send error \n ");
	}
	return re;
}

int recvEchoReQuest(SOCKET sock,ECHORESPONSE * sponse, sockaddr_in *dstIP) {
	socklen_t size = sizeof(sockaddr);
	int re = recvfrom(sock, (char *)sponse, sizeof(ECHORESPONSE), 0,(struct sockaddr*)dstIP, &size);
	if (-1 == re)
	{
		perror("recvfrom error");
	}
	return re;
}

u_short checksum(u_short *buffer, int len) {
	register int nleft = len;
	register u_short *w = buffer;
	register u_short answer;
	register int sum = 0;
	// 使用32bit的累加器，进行16bit的反馈计算
     while (nleft > 1) {
		sum += *w++;
		nleft -= 2;
	}
	// 补全奇数位
	if (nleft == 1) {
		u_short u = 0;
	
		*(u_char *)(&u) = *(u_char *)w;
		sum += u;
	}
	// 将反馈的16bit从高位移至地位
	sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
	sum += (sum >> 16);                 /* add carry */
	answer = ~sum;                      /* truncate to 16 bits */
	return (answer);
}
