#include <pthread.h>
///thread create function
int dk_thread_func(void (*func)(void)) {
	pthread_t thd;
	pthread_create(&thd,NULL,(void * _Nullable(* _Nonnull)(void * _Nullable))func,NULL);	
	pthread_detach(thd);
	return 1;
}

///master thread
void dk_master_thread() {
	
}

///worker thread
void dk_worker_thread() {
	
}

/// start tcp_server
//  param: work_count  worker thread count
//  	   workers work in a thread pool,
//  	   init work_count size threads
int dk_start(int worker_count = 1) {
	int master_stat = dk_thread_func(dk_master_thread);	
	for(int i = 0; i < worker_count; ++i)
		dk_thread_func(dk_worker_thread);
	return master_stat;
}

