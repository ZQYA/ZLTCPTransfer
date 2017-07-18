#ifndef __DK_TOOL__
#define __DK_TOOL__
#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <fstream>
#include <ostream>
#include <map>
#include <sys/stat.h>
#define L_DEBUG     0 
#define L_INFO   	1
#define L_WARNING 	2
#define L_ERROR  	3
class dk_output_stream {
public:
	dk_output_stream(int _level):level(_level) {
	} 
	~dk_output_stream() {
		(*this)<<std::endl; /// flush all the file output stream
	}	
	
	template <typename T> dk_output_stream & operator<<(const  T t) {
		if(this->level >= L_ERROR) {
			*error_stream<<t;
			//error_stream->flush();
        }
        if(this->level >= L_WARNING){
            *warning_stream<<t;
			//warning_stream->flush();
		}
        if(this->level >= L_INFO){
            *info_stream<<t;
			//info_stream->flush();
		}
        if(this->level >= L_DEBUG ) {
            std::cout<<t;    
        }
		return *this;
	}  
	
	dk_output_stream & operator<<(std::ostream &(*)(std::ostream &)) {
		if(this->level >= L_ERROR) {
			*error_stream<<"\n";
			error_stream->flush();
		}
		if(this->level >= L_WARNING){
			*warning_stream<<"\n";
			warning_stream->flush();
		}
		if(this->level >= L_INFO){
			*info_stream<<"\n";
			info_stream->flush();
		}
		if(this->level >= L_DEBUG ) {
			std::cout<<std::endl;    
		}
		return *this;	
	}

private:
	int level; 
	static std::ostream* dk_create_ostream(int level) {
		const char *name = level>=L_ERROR?"/dk_error.log":(level>=L_WARNING?"/dk_warning.log":(level>=L_INFO?"/dk_info.log":NULL));	
		if(name == NULL) {
			return NULL;	
		}else {
			char *file_path = (char *)malloc(1024);
			bzero(file_path,1024);
			char *home_path = getenv("HOME");	
			const char *level_path = "/dk_logd";
			strcat(file_path,home_path);
			strcat(file_path,level_path);
			if(0!=access(file_path,F_OK)) {
				int res = mkdir(file_path,0777);			
				if(res!=0) {
					std::cerr<<"create log path failed"<<file_path<<std::endl;
				}
			}
			strcat(file_path,name);
			std::ofstream *fstm = new std::ofstream(std::string(file_path),std::ios::app);
			free(file_path);
			return fstm;
		}
	}
	static std::ostream *error_stream ;   
	static std::ostream *warning_stream; 
	static std::ostream *info_stream;
};
#define LOG_DEBUG  LOG(L_DEBUG)
#define LOG_INFO   LOG(L_INFO)
#define LOG_WARNING LOG(L_WARNING)
#define LOG_ERROR LOG(L_ERROR)
std::string dk_prefix_log(int level);
#define LOG(level) \
	dk_output_stream(level)<<dk_prefix_log(level)<<":"<<__FILE__<<":"<<__func__<<":"<<__LINE__<<":"
#endif



