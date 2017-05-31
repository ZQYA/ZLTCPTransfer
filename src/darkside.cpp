#include <pthread.h>
#include <sys/socket.h>
#include "dktool.hpp"
#include <cstdlib>


//// macros 
#define dk_check(val) if(-1 == val) exit(1)
/// constants


/// vars
int dk_listen_port = 0;

///thread create function
int dk_thread_func(void (*func)(void)) {
	pthread_t thd;
	pthread_create(&thd,NULL,(void * _Nullable(* _Nonnull)(void * _Nullable))func,NULL);	
	pthread_detach(thd);
	return 1;
}

///master thread
void dk_master_thread(void) {
	SOCKET sk_fd = dk_socket();
	dk_check(sk_fd);
	sockaddr_in server_addr;
	int stat = dk_bind(sk_fd,dk_listen_port,&server_addr);
	dk_check(stat);
	stat = dk_listen(sk_fd);
	dk_check(stat);
//TODO: multple I/O need support.
	stat = dk_accept(sk_fd,&server_addr);
	dk_check(stat);
} 
///worker thread
void dk_worker_thread(void) {
	
}

/// start tcp_server
//  param: work_count  worker thread count
//  	   workers work in a thread pool,
//  	   init work_count size threads
int dk_start(int worker_count = 1, int listen port) {
	dk_listen_port = listen_port;
	int master_stat = dk_thread_func(dk_master_thread);	
	for(int i = 0; i < worker_count; ++i)
		dk_thread_func(dk_worker_thread);
	return master_stat;
}

