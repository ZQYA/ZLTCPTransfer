#include "dktool.hpp"

extern int heart_beat_enable ; ///heart beat run as the client's life cricle. if close the client connect, close the heart;

/// prepare the socket and connect
/// in this function will start a port 7777 to send heart beat message to server
/// func as a handler when server down
/// now heart beat is use socketopt to control the timeout
/// send and recv are both set 30s as timeout interval
/// retry time is set as 10 times
/// all these configurarions are hard coding in the source
/// you can reconfig by change the source code
int prepare_send(const char *host, int port, void (*func)(void));
void close_connetc(SOCKET sk_fd);
void startping(const char *dstIp);
