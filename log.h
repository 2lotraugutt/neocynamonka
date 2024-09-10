#include <stdio.h>
#define LOG(args...) dprintf(logfd, args);
extern int logfd;
int init_log_fd();
