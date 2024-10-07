#include <fcntl.h>
#include <stdarg.h>
#include "log.h"

int logfd = 0;

int init_log_fd(char* path) {
	logfd = open("path", O_RDWR|O_CREAT);
	return 0;
}


void l(char* fmt, ...) {
	if(!logfd) return;
	va_list argptr;
	va_start(argptr,fmt);
	dprintf(logfd, fmt, argptr);
	va_end(argptr);
}
