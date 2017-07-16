#include "dklog.hpp"
#include <cstdio>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
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
	std::ostream *error_stream;   
	std::ostream *warning_stream;
	std::ostream *info_stream;
public:
	dk_output_stream(int _level):level(_level) {
		std::cout << "level:" << level << std::endl;
	    this->error_stream = this->dk_create_ostream(L_ERROR);
        this->warning_stream = this->dk_create_ostream(L_WARNING);
        this->info_stream = this->dk_create_ostream(L_INFO);
	} 
	
	
	template <typename T> dk_output_stream & operator<<(const  T t) {
		if(this->level >= L_ERROR) {
			*error_stream<<t;
        }
        if(this->level >= L_WARNING){
            *warning_stream<<t;
		}
        if(this->level >= L_INFO){
            *info_stream<<t;
		}
        if(this->level >= L_DEBUG ) {
            std::cout<<t;    
        }
	}  


	std::ostream* dk_create_ostream(int level) const {
		const char *name = level>=L_ERROR?"/dk_error.log":(level>=L_WARNING?"/dk_warning":(level>=L_INFO?"/dk_info":NULL));	
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
			std::ofstream *fstm = new std::ofstream(std::string(file_path),std::ios::out);
			free(file_path);
			return fstm;
		}
	}
};

int main(int args , const char **argv) {
    dk_output_stream s(L_ERROR);
    s<<"test error"<<3<<"nuil"; 
    return 0;
}
