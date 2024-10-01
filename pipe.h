#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

extern int pipefd;

int setup_pipe(char* path);
void*init_pipe(void* NONE);
