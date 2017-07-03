#include <pthread.h>
/// start tcp_server
//  param: work_count  worker thread count
//  	   workers work in a thread pool,
//  	   init work_count size threads
  	   

int dk_start(int worker_count ,  int listen_sock_count , int listen_port,int heartbeat_port) ;
void dk_stop();


