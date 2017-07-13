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
#include <algorithm>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
extern "C" { 
#include "lua_src/src/lua.h"
#include "lua_src/src/lauxlib.h"
#include "lua_src/src/lualib.h"
}
#include <iostream>
//// macros 
#define dk_check(val) if(-1 == val) exit(1)
/// constants

#define MAXSOCK(SOCKA,SOCKB) ((SOCKA)>(SOCKB)?(SOCKA):(SOCKB))

/// vars
int dk_listen_port = 8000; // server default port
int dk_heartbeat_port = 10001; //server's life port
size_t dk_accept_max_count = 1;
bool dk_start_flag = false;// indicate server running state 
volatile sig_atomic_t sig_status;
lua_State *L;
pthread_mutex_t receive_queue_full_lock = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t receive_queue_empty_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t receive_queue_full_sig = PTHREAD_COND_INITIALIZER;
pthread_cond_t receive_queue_empty_sig = PTHREAD_COND_INITIALIZER;
std::list<SOCKET> sock_list;
SOCKET  listen_sock_fd;
SOCKET  heartbeat_sock_fd;

typedef struct dk_file_path {
	int fd;
	char *filename;
}dk_file_path;

std::map<SOCKET,dk_file_path>sock_data_map; 

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
	int max_fd = std::max(listen_sock_fd,heartbeat_sock_fd)+1;	
//TODO: multple I/O need support.
	while(dk_start_flag){
		FD_ZERO(&read_set);
		FD_SET(listen_sock_fd,&read_set);
		FD_SET(heartbeat_sock_fd,&read_set);
		tv.tv_sec = 0;
		tv.tv_usec = 200 * 1000;
		if(select(max_fd,&read_set,NULL,NULL,&tv)>0) {
			if(dk_start_flag&&FD_ISSET(listen_sock_fd,&read_set)) {
				accecpt_new_connection(listen_sock_fd);										
			}else if (dk_start_flag && FD_ISSET(heartbeat_sock_fd,&read_set)) {
				accecpt_new_connection(heartbeat_sock_fd);
			}
		}
	}
}

void dk_handle_msg(mmtp mp, SOCKET sk_fd) {
	char *message = (char *)malloc(mp.content_length+1);
	memcpy(message,mp.content,mp.content_length);
	printf("%s",message);
    if (0 == strcmp(message, "heartbeat")) {
        ssize_t back_size = mp_write(sk_fd, "heartbeat", sizeof("heartbeat"), 0, 1,NULL);
        if (back_size == 0) {
            dk_perror("back say error");
        }
    
    }
	bzero(message,mp.content_length+1);
	free(message);
}

bool check_image_in_path(lua_State *L,const char *file_path);
void dk_handle_img(mmtp mp, SOCKET sk_fd) {
	char *img_data = (char *)malloc(mp.content_length);		
	bzero(img_data,mp.content_length);
	memcpy(img_data,mp.content,mp.content_length);
	
	int fd = sock_data_map[sk_fd].fd;
	ssize_t re = write(fd,img_data,mp.content_length);
	if(re < 0) {
		dk_perror("write failed");
	}else {
		if(mp.is_end) {
			bool isimg = check_image_in_path(L,sock_data_map[sk_fd].filename);			
			if(!isimg) {
				printf("check img failed");	
			}
		}
	}
}

void dk_handle_video(mmtp mp, SOCKET sk_fd) {
	char *img_data = (char *)malloc(mp.content_length);		
	bzero(img_data,mp.content_length);
	memcpy(img_data,mp.content,mp.content_length);
	int fd = sock_data_map[sk_fd].fd;
	ssize_t re = write(fd,img_data,mp.content_length);
	if(re < 0) {
		dk_perror("write failed");
	}
}
void dk_handle_mmtp(mmtp mp, SOCKET sk_fd)  {
	if((sock_data_map.find(sk_fd) == sock_data_map.end() || mp.is_first )) {
		const char *home = getenv("HOME");
		char *home_path = (char *)malloc(1024);
		bzero(home_path,1024);
		strcat(home_path,home);
		if(mp.type!=0) {
			strcat(home_path,"/");
			strcat(home_path,mp.options);
		}
		if(0 != access(home_path,F_OK)) {
			int res = mkdir(home_path, 0777);
			if (res!=0) {
				dk_perror("create dir field");
			}
		}
		strcat(home_path,0==mp.type?"/log":(1==mp.type?"/img":"/vdo"));
		strcat(home_path,"XXXXXXXX");
		char *tmpfile = strdup(mktemp(home_path)); 
		free(home_path);
		if(mp.type!= 0) {
			int fd = open(tmpfile,O_CREAT|O_APPEND|O_RDWR,0777);
			dk_file_path fp = {fd,tmpfile};
			sock_data_map[sk_fd] = fp;
		}else {
			free(tmpfile);
		}
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
						if(size <= 0) {
							f_ds[f_di++] = sk_fd;		
						}else {
							dk_handle_mmtp(mp, sk_fd);
						}
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

SOCKET create_listen_socks(SOCKET *heartbeat_sock_fd) {
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
	SOCKET result = sk_fd;
	sk_fd = dk_socket();
	if(sk_fd>0) {
		sockaddr_in server_addr;
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(dk_heartbeat_port);
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		int stat = dk_bind(sk_fd,dk_heartbeat_port,&server_addr);
		dk_check(stat);
		stat = dk_listen(sk_fd);
		*heartbeat_sock_fd = sk_fd;
		dk_check(stat);
	}
	return result;
}

/// create mem copy ptr for the performance consideration
/// now server just operate the maped file's mem ptr to access file
void* build_map_ptr() {		
	const char* home_file_path = getenv("HOME");
	const char *filepath = "/device_ip_pair.log";	
	char *map_file_path= (char *)malloc(1024);
	strcat(map_file_path,home_file_path);
	strcat(map_file_path,filepath);
	int map_fd = open(map_file_path,O_CREAT|O_RDWR,0777);
	if(-1 == map_fd) {
		perror("file des create failed");
	}
	void *ptr = mmap(NULL,4*1024*8,PROT_WRITE|PROT_READ,MAP_SHARED,map_fd,0);
	return ptr;
}


/// start tcp_server
//  param: work_count  worker thread count
//  	   workers work in a thread pool,
//  	   init work_count size threads
int dk_start(int worker_count = 1,  int listen_sock_count = 6, int listen_port = 9000,int heartbeat_port = 10001) {
	dk_deamonInit();
	L = luaL_newstate();
	luaL_openlibs(L);
	int re = luaL_loadfile(L,"./imgck.lua")||lua_pcall(L,0,0,0);
	if(re) {
		const char *err = lua_tostring(L,-1);
		printf("load lua error:%s",err);
	}
	void *ptr = build_map_ptr();
	dk_start_flag = true;
	dk_listen_port = listen_port;
	dk_heartbeat_port = heartbeat_port;
	dk_accept_max_count = listen_sock_count;
	listen_sock_fd = create_listen_socks(&heartbeat_sock_fd);
	for(int i = 0; i < worker_count; ++i)
		dk_thread_func(dk_worker_thread);
	int master_stat = dk_thread_func(dk_master_thread,false);	
	return master_stat;
}
bool check_image_in_path(lua_State *L,const char *file_path) {
	lua_getglobal(L,"checkimg");
	lua_pushstring(L,file_path);
	if(lua_pcall(L,1,1,0)!=0) {
		printf("lua func invoke failed:%s\n",lua_tostring(L,-1));
	}else if(lua_isnumber(L,-1)){
		int isimg = lua_toboolean(L,-1);
		return isimg;
	}
	return false;
}

void dk_stop() {
	dk_start_flag = false;
} 

