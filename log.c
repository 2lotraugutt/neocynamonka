#include <fcntl.h>
#include "log.h"
int logfd;
int init_log_fd() {
	logfd = open("neocynamonka.log", O_RDWR|O_CREAT);
	return 0;
}
