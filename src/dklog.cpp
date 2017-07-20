#include "dklog.hpp"
#include <time.h>
#include <execinfo.h>
/********************************************************************************
 * define a class to process the log
 * every level has a file outputstream , higher level will cover the lower level's 
 * file opertion
 * etc: error level log will write the error file, warning file, info file and std
 * -out 
 * the debug only has stdout as its outputstream 
 * the class overwrite the << operator, so you can use << "" << "" as a link 
********************************************************************************/
class dk_output_stream ;

std::ostream *dk_output_stream::error_stream = dk_output_stream::dk_create_ostream(L_ERROR);   
std::ostream *dk_output_stream::warning_stream = dk_output_stream::dk_create_ostream(L_WARNING);
std::ostream *dk_output_stream::info_stream = dk_output_stream::dk_create_ostream(L_INFO);

dk_output_stream s(L_ERROR);

std::string dk_prefix_log(int level) {
	std::string s;
	switch(level) { 
		case L_DEBUG: 
				s+="[DEBUG]";
			break;
		case L_INFO:
				s+="[INFO]";
			break;
		case L_WARNING:
				s+="[WARNING]";
			break;
		case L_ERROR:
				s+="[ERROR]";
			break;
	}
	time_t t= time(NULL);
	if(t!=-1) {
		char *asct = asctime(localtime(&t));
		size_t asctsize = strlen(asct);
		asct[asctsize-1]='\0';
		s+=asct;
	}
	return s;
}

std::string backtrace_str() {
	std::string b_st;
	void* callstack[128];
	int i,frames = backtrace(callstack,128);
	char **strs = backtrace_symbols(callstack,frames);
	for (i = 0; i < frames; ++i) {
		b_st += std::string(strs[i]);
	}
	return b_st;
}
//int main(int args , const char **argv) {
//  LOG_WARNING<<"test warning"<<3<<"nuil"<<std::endl;
//   LOG_ERROR<<"test error"<<3<<"nuil"<<std::endl;
//  return 0;
//}

