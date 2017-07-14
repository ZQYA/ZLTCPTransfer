#ifndef __DK_TOOL__
#include <iostream>
#define __DK_TOOL__
#define L_DEBUG     0 
#define L_INFO   	1
#define L_WARNING 	2
#define L_ERROR  	3
std::ostream& LOG(int level);
#define LOG_DEBUG  LOG(L_DEBUG)
#define LOG_INFO   LOG(L_INFO)
#define LOG_WARNING LOG(L_WARNING)
#define LOG_ERROR LOG(L_ERROR)
#endif



