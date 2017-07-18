#include "dklog.hpp"
#include <cstdio>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#include <ostream>
#include <map>
#include <time.h>
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
public:
	dk_output_stream(int _level):level(_level) {
	    this->error_stream = this->dk_create_ostream(L_ERROR);
        this->warning_stream = this->dk_create_ostream(L_WARNING);
        this->info_stream = this->dk_create_ostream(L_INFO);
	} 
	~dk_output_stream() {
		(*this)<<std::endl; /// flush all the file output stream
		free(this->error_stream);
		free(this->warning_stream);
		free(this->info_stream);
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
	
	dk_output_stream & operator<<(std::ostream &(*f)(std::ostream &)) {
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
	std::ostream *error_stream;   
	std::ostream *warning_stream;
	std::ostream *info_stream;
	std::ostream* dk_create_ostream(int level) const {
		const char *name = level>=L_ERROR?"/dk_error.log":(level>=L_WARNING?"/dk_warning.log":(level>=L_INFO?"/dk_info":NULL));	
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
};

dk_output_stream LOG(int level) {
	dk_output_stream s(level);
	time_t t= time(NULL);
	if(t!=-1) {
		char *asct = asctime(localtime(&t));
		size_t asctsize = strlen(asct);
		asct[asctsize-1]='\0';
		s<<asct<<": ";
	}
	return s;
}

int main(int args , const char **argv) {
   LOG_WARNING<<"test warning"<<3<<"nuil"<<std::endl;
   return 0;
}

