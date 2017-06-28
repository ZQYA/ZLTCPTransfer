#include <pthread.h>
#include <sys/socket.h>
#include <sys/select.h>
#include "dktool.hpp"
#include <cstdlib>
#include <unistd.h>
#include <csignal>
#include "luke.hpp"
#include <arpa/inet.h>
#include <list>
#include <stdio.h>
#include <fcntl.h>
#include <map>
//// macros 
#define dk_check(val) if(-1 == val) exit(1)
/// constants

#define MAXSOCK(SOCKA,SOCKB) ((SOCKA)>(SOCKB)?(SOCKA):(SOCKB))

/// vars
int dk_listen_port = 8000; // server default port
size_t dk_accept_max_count = 1;
bool dk_start_flag = false;// indicate server running state 
volatile sig_atomic_t sig_status;
pthread_mutex_t receive_queue_full_lock = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t receive_queue_empty_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t receive_queue_full_sig = PTHREAD_COND_INITIALIZER;
pthread_cond_t receive_queue_empty_sig = PTHREAD_COND_INITIALIZER;
std::list<SOCKET> sock_list;
SOCKET  listen_sock_fd;

std::map<SOCKET,const char *> sock_data_map; 

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

void produce_socket(SOCKET s) {
	pthread_mutex_lock(&receive_queue_empty_lock);					
	sock_list.push_back(s);	
	pthread_cond_signal(&receive_queue_full_sig);
	pthread_mutex_unlock(&receive_queue_empty_lock);	
}


void accecpt_new_connection(SOCKET sock) {
	sockaddr_in server_addr;
	SOCKET accept_sock = dk_accept(sock,&server_addr);
	produce_socket(accept_sock);
}


///master thread
void dk_master_thread(void) {
#ifndef DK_THREAD
    signal(SIGCHLD,childprocess_exit_handler);        
#endif
	fd_set read_set;
	struct timeval tv;
	int max_fd = listen_sock_fd+1;	
//TODO: multple I/O need support.
	while(dk_start_flag){
		FD_ZERO(&read_set);
		FD_SET(listen_sock_fd,&read_set);
		tv.tv_sec = 0;
		tv.tv_usec = 200 * 1000;
		if(select(max_fd,&read_set,NULL,NULL,&tv)>0) {
			if(dk_start_flag&&FD_ISSET(listen_sock_fd,&read_set)) {
				accecpt_new_connection(listen_sock_fd);										
			}	
		}
	}
}

void dk_handle_msg(mmtp mp, SOCKET sk_fd) {
	char *message = (char *)malloc(mp.content_length+1);
	memcpy(message,mp.content,mp.content_length);
	printf("%s",message);
	bzero(message,mp.content_length+1);
	free(message);
}

void dk_handle_img(mmtp mp, SOCKET sk_fd) {
	char *img_data = (char *)malloc(mp.content_length);		
	bzero(img_data,mp.content_length);
	memcpy(img_data,mp.content,mp.content_length);
	int fd = open(sock_data_map[sk_fd],O_RDWR|O_CREAT|O_APPEND);
	int re = write(fd,img_data,mp.content_length);
	if(re < 0) {
		perror("write failed");
	}
}

void dk_handle_video(mmtp mp, SOCKET sk_fd) {
	char *img_data = (char *)malloc(mp.content_length);		
	bzero(img_data,mp.content_length);
	memcpy(img_data,mp.content,mp.content_length);
	int fd = open(sock_data_map[sk_fd],O_RDWR|O_CREAT|O_APPEND);
	write(fd,img_data,mp.content_length);
}
void dk_handle_mmtp(mmtp mp, SOCKET sk_fd)  {
	if(sock_data_map.find(sk_fd) == sock_data_map.end()) {
		const char *home = getenv("HOME");
		char *tmpfile = strdup(tempnam(home, mp.type==0?"msg":(mp.type==1?"img":"video")));
		sock_data_map[sk_fd] = tmpfile;
	}
	switch (mp.type) {
		case 0: 
			dk_handle_msg(mp, sk_fd);
			break;
		case 1: 
			dk_handle_img(mp,sk_fd);
			break;
		case 2: 
			dk_handle_video(mp,sk_fd);
			break;
	}		
}

///worker thread
void dk_worker_thread(void) {
	while(dk_start_flag) {
			pthread_mutex_lock(&receive_queue_empty_lock);					
			while(dk_start_flag && sock_list.size()<=0){
				pthread_cond_wait(&receive_queue_full_sig,&receive_queue_empty_lock);	
			}		
			
			fd_set read_set;
			struct timeval tv;
			tv.tv_sec = 1;
			tv.tv_usec = 200*1000;
			int max_fd = -1;
			FD_ZERO(&read_set);
			for(std::list<SOCKET>::iterator it = sock_list.begin(); it != sock_list.end(); ++it) {
				SOCKET sk_fd = *it;	
				FD_SET(sk_fd,&read_set);
				max_fd = MAXSOCK(max_fd,sk_fd);	
			}
			SOCKET *f_ds = new SOCKET[sock_list.size()];
			int f_di = 0;
			if (select(max_fd+1,&read_set,NULL,NULL,&tv)>0) {
				for(std::list<SOCKET>::iterator it = sock_list.begin(); it != sock_list.end(); ++it) {
					SOCKET sk_fd = *it;	
					if(FD_ISSET(sk_fd,&read_set)) {
						struct mmtp mp;
						initilizer_mmtp(&mp);
						int size = mp_read(sk_fd,0,&mp);
						if(size == 0) {
							f_ds[f_di++] = sk_fd;		
						}
						dk_handle_mmtp(mp, sk_fd);
//						FD_CLR(sk_fd,&read_set);
					}
				}
			}
			//// delete closed sock
			for(std::list<SOCKET>::size_type i = 0; i < sock_list.size();++i) {
				if(f_ds[i]!=0) {
					sock_list.remove(f_ds[i]);		
				}
				sock_data_map.erase(f_ds[i]);
			}
			pthread_mutex_unlock(&receive_queue_empty_lock);	
	}
}


SOCKET create_listen_socks() {
	SOCKET sk_fd= dk_socket(); 		
	if(sk_fd>0) {
		sockaddr_in server_addr;
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(dk_listen_port);
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		int stat = dk_bind(sk_fd,dk_listen_port,&server_addr);
		dk_check(stat);
		stat = dk_listen(sk_fd);
		dk_check(stat);
	}
	return sk_fd;
}


/// start tcp_server
//  param: work_count  worker thread count
//  	   workers work in a thread pool,
//  	   init work_count size threads
int dk_start(int worker_count = 1,  int listen_sock_count = 6, int listen_port = 9000) {
//	dk_deamonInit();
	dk_start_flag = true;
	dk_listen_port = listen_port;
	dk_accept_max_count = listen_sock_count;
	listen_sock_fd = create_listen_socks();
	//for(int i = 0; i < worker_count; ++i)
		dk_thread_func(dk_worker_thread);
	int master_stat = dk_thread_func(dk_master_thread,false);	
	return master_stat;
}

void dk_stop() {
	dk_start_flag = false;
} 
