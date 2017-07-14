#include "dklog.hpp"
#include <cstdio>
/********************************************************************************
 * define a class to process the log
 * every level has a file outputstream , higher level will cover the lower level's 
 * file opertion
 * etc: error level log will write the error file, warning file, info file and std
 * -out 
 * the debug only has stdout as its outputstream 
 * the class overwrite the << operator, so you can use << "" << "" as a link 
********************************************************************************/
class dk_output_stream {
private:
	int level; 
	std::ostream error_stream;   
	std::ostream warning_stream;
	std::ostream info_stream;
public:
	dk_output_stream(int _level):level(_level) const {
		std::cout << "level:" << level << std::endl;
		const char *home_path  = getenv("HOME");
		const char *log_dir = "/dk_log";
		char *log_path_in_home = (char *)malloc(1024);
		
	} 
	
	
	dk_output_stream & operator<T><<(const  T t) {
		if(this->level >= L_ERROR) {
				
		}	
	}  


	std::ostream dk_create_ostream(int level) const {
		char *name = level>=L_ERROR?"/dk_error.log":(level>=L_WARNING?"/dk_warning":(level>=L_INFO?"/dk_info":NULL));	
		if(name == NULL) {
			return NULL;	
		}else {
			char *file_path = (char *)malloc(1024);
			bzero(file_path,1024);
			char *home_path = getenv("HOME");	
			char *level_path = "/dk_logd";
			strcat(file_path,home_path);
			strcat(file_path,level_path);
			if(0!=access(file_path,F_OK)) {
				int res = mkdir(filepath,0777);			
				if(res!=0) {
					std::cerr<<"create log path failed"<<file_path<<std::endl;
				}
			}
			strcat(file_path,name);
			std::ofstream fstm(file_path);
			free(file_path);
			return fstm;
		}
	}
}


void log(int level,char *fmt, ...) {	
	switch (level) { 
			case L_DEBUG:
				break;
			case L_INFO:
				break;
			case L_WARNING:
				break;
			case L_ERROR:
				break;
	}
}
