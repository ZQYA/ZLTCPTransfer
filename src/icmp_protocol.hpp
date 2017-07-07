#include <netinet/in.h>
#include "dktool.hpp"

/// ICMP Header
typedef struct _ICMPHeader {
	u_char type;
	u_char code;
	u_short ck_sum;
	u_short id;
	u_short seq;
} ICMPHeader,*PICMPHDR;

typedef struct _ECHOREQUEST {
	ICMPHeader icmpHeader;
	int time;
	char data[32];
} ECHOREQUEST, *PECHOREQUEST;

typedef struct _ECHORESPONSE {
	ECHOREQUEST echoRequest;
	char fill[255];
} ECHORESPONSE,*PECHORESPONSE;


ssize_t sendEchoRequest(SOCKET sock , sockaddr_in detIP);
ssize_t recvEchoReQuest(SOCKET sock,ECHORESPONSE * sponse, sockaddr_in *dstIP);
u_short checksum(u_short *buffer, int len);