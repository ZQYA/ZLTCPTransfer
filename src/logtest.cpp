#include <syslog.h>
int main() {
	openlog("DKSIDE",LOG_CONS|LOG_PID,LOG_SYSLOG);
	syslog(LOG_INFO,"TEST MESSAGE FORM TEST PROCESS\n");
	syslog(LOG_INFO,"TEST MESSAGE FORM TEST PROCESS\n");
	syslog(LOG_INFO,"TEST MESSAGE FORM TEST PROCESS\n");
	return 0;
}
