#include <pthread.h>
#include <sys/socket.h>
#include <sys/select.h>
#include "dktool.hpp"
#include <cstdlib>
#include <unistd.h>
#include <csignal>
#include "luke.hpp"
#include <arpa/inet.h>
//// macros 
#define dk_check(val) if(-1 == val) exit(1)
/// constants

#define MAXSOCK(SOCKA,SOCKB) ((SOCKA)>(SOCKB)?(SOCKA):(SOCKB))
typedef struct listening_sock {
	SOCKET sock;
	struct listening_sock *next;
	listening_sock (SOCKET s, struct listening_sock *n) : sock(s),next(n){
		printf("init sock :%d\n",s);
	}
} listening_sock;

/// vars
int dk_listen_port = 8000; // server default port
bool dk_start_flag = false;// indicate server running state 
volatile sig_atomic_t sig_status;
pthread_mutex_t receive_queue_full_lock = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t receive_queue_empty_lock = PTHREAD_MUTEX_INITIALIZER;
listening_sock *sock_header = NULL;
#define DK_THREAD 
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
int dk_thread_func(void (*func)(void), bool detach=true) {
	pthread_t thd;
	pthread_create(&thd,NULL,(void * _Nullable(* _Nonnull)(void * _Nullable))func,NULL);	
	int *result = new int(0);
	if(detach){
		pthread_detach(thd);
		return 0;
	}
	else {
		pthread_join(thd,(void **)&result);
		int res = *result;
		delete result;
		return res;
	}
}
#ifndef DK_THREAD
void childprocess_exit_handler(int signal) {
	sig_status = signal; 
	pid_t pid;	   
	int stat;
	while((pid = waitpid(-1,&stat,WNOHANG))>0) {
		printf("child process exit:%d",pid);	
	}
}
#endif

void accecpt_new_connection(SOCKET sock) {
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(dk_listen_port);
	server_addr.sin_addr.s_addr = inet_addr(INADDR_ANY);
	SOCKET accept_sock = dk_accept(sock,&server_addr);
}


///master thread
void dk_master_thread(void) {
#ifndef DK_THREAD
    signal(SIGCHLD,childprocess_exit_handler);        
#endif
	fd_set read_set;
	struct timeval tv;
	int max_fd;
//TODO: multple I/O need support.
	while(dk_start_flag){
		FD_ZERO(&read_set);
		max_fd = -1;	
		for (listening_sock *sock = sock_header; sock!=NULL; sock = sock->next) {
			FD_SET(sock->sock,&read_set);
			max_fd = MAXSOCK(max_fd,sock->sock);			
		}
		tv.tv_sec = 0;
		tv.tv_usec = 200 * 1000;
		if(select(max_fd+1,&read_set,NULL,NULL,&tv)>0) {
			for (listening_sock *sock = sock_header; sock!=NULL; sock = sock->next) {
				if(dk_start_flag&&FD_ISSET(sock->sock,&read_set)) {
					accecpt_new_connection(sock->sock);										
				}	
			}
		}
#ifndef DK_THREAD
		pid_t pid = fork();
		if (pid == 0) {  /// start child process
			dk_check(stat);
			handleSocket(con_so);	
			exit(0);
		}else {
			close(con_so);
		}
#else 
			
#endif
	}
} 

///worker thread
void dk_worker_thread(void) {
		
}


listening_sock *create_listen_socks(int count) {
	listening_sock *ls = new listening_sock(0, NULL);
	listening_sock *tmp = ls;
   	for(int i = 0; i < count; ++i) {
		SOCKET sk_fd= dk_socket(); 		
		if(sk_fd>0) {
			sockaddr_in server_addr;
			server_addr.sin_family = AF_INET;
			server_addr.sin_port = htons(dk_listen_port);
			server_addr.sin_addr.s_addr = inet_addr(INADDR_ANY);
			int stat = dk_bind(sk_fd,dk_listen_port,&server_addr);
			dk_check(stat);
			stat = dk_listen(sk_fd);
			dk_check(stat);
			if(0 == tmp->sock) {
				tmp->sock = sk_fd;		
			}else {
				listening_sock *s = new listening_sock(sk_fd,NULL);
				tmp->next = s;
				tmp = s;
			}	
		}
	}	return ls;
}


void destory_listen_socks(listening_sock *header) {
	while(header!=NULL) {
		header = header->next;
		free(header);
	}	
}

/// start tcp_server
//  param: work_count  worker thread count
//  	   workers work in a thread pool,
//  	   init work_count size threads
int dk_start(int worker_count = 1,  int listen_sock_count = 6, int listen_port = 9000) {
	dk_start_flag = true;
	dk_listen_port = listen_port;
	sock_header = create_listen_socks(listen_sock_count);
#ifdef DK_THREAD
	for(int i = 0; i < worker_count; ++i)
		dk_thread_func(dk_worker_thread);
#endif
	int master_stat = dk_thread_func(dk_master_thread,false);	
	return master_stat;
}

void dk_stop() {
	dk_start_flag = false;
} 
