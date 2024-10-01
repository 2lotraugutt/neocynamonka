#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int setup_pipe() {
	int fd = open("/usr/share/neocynamonka/pipe", O_WRONLY|O_NONBLOCK);
	write(fd, "Hello World", sizeof("Hello World")-1);
	close(fd);
	return 0;
}
