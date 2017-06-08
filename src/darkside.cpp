#include <pthread.h>
#include <sys/socket.h>
#include "dktool.hpp"
#include <cstdlib>
#include <unistd.h>
#include <csignal>
#include "luke.hpp"
//// macros 
#define dk_check(val) if(-1 == val) exit(1)
/// constants


/// vars
int dk_listen_port = 8000; // server default port
bool dk_start_flag = false;// indicate server running state 
volatile sig_atomic_t sig_status;

#ifndef DK_THREAD 
void handleSocket(SOCKET sock) {		
	char *buffer = (char *)malloc(4);
	bzero(buffer,4);
	struct mmtp mp;
	initilizer_mmtp(&mp);
	int size = mp_read(sock,0,&mp);
	char *message = (char *)malloc(mp.content_length+1);
	bzero(message,mp.content_length+1);
	memcpy(message,mp.content,mp.content_length);
   	printf("%s",message);	
	free(buffer);
}
#endif
///thread create function
int dk_thread_func(void (*func)(void)) {
	pthread_t thd;
	pthread_create(&thd,NULL,(void * _Nullable(* _Nonnull)(void * _Nullable))func,NULL);	
	pthread_detach(thd);
	return 1;
}
void childprocess_exit_handler(int signal) {
	sig_status = signal; 
	pid_t pid;	   
	int stat;
	while((pid = waitpid(-1,&stat,WNOHANG))>0) {
		printf("child process exit:%d",pid);	
	}
}
///master thread
void dk_master_thread(void) {
#ifndef DK_THREAD
    signal(SIGCHLD,childprocess_exit_handler);        
#endif
	SOCKET sk_fd = dk_socket();
	dk_check(sk_fd);
	sockaddr_in server_addr;
	int stat = dk_bind(sk_fd,dk_listen_port,&server_addr);
	dk_check(stat);
	stat = dk_listen(sk_fd);
	dk_check(stat);
//TODO: multple I/O need support.
	while(dk_start_flag){
		SOCKET con_so = dk_accept(sk_fd,&server_addr);
		handleSocket(con_so);
//#ifndef DK_THREAD
//		pid_t pid = fork();
//		if (pid == 0) {  /// start child process
//			dk_check(stat);
//			handleSocket(con_so);	
//			exit(0);
//		}else {
//            close(con_so);
//       }
//#endif
	}
} 

///worker thread
void dk_worker_thread(void) {
		
}

/// start tcp_server
//  param: work_count  worker thread count
//  	   workers work in a thread pool,
//  	   init work_count size threads
int dk_start(int worker_count = 1, int listen_port=9000) {
	dk_start_flag = true;
	dk_listen_port = listen_port;
	int master_stat = dk_thread_func(dk_master_thread);	
#ifndef DK_THREAD
	for(int i = 0; i < worker_count; ++i)
		dk_thread_func(dk_worker_thread);
#endif
    while(1) {}
	return master_stat;
}

void dk_stop() {
	dk_start_flag = false;
} 
