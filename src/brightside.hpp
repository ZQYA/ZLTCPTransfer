#include "dktool.hpp"

extern int heart_beat_enable ; ///heart beat run as the client's life cricle. if close the client connect, close the heart;
int prepare_send(const char *host, int port) ;
void close_connetc(SOCKET sk_fd);
void startping(const char *dstIp);
