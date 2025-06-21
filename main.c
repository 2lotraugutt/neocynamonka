#include <pthread.h>
#include <unistd.h>
#include "glob.h"
#include "log.h"
#include "setup.h"
#include "draw.h"
#include "ping.h"

int main(int argc, char **argv) {
	uint32_t pid = getpid();

  if(parse_cmdarg(argc, argv)) return -1;
  if(parse_config(config_file)) return -1;

	pthread_attr_t t_attr;
	CRIT_HANDLE(pthread_attr_init(&t_attr), "pthread attr init");

	pthread_t ping_t, listen_t, draw_t, pipe_t;

	pthread_create(&listen_t, &t_attr, &listener, &pid);
	pthread_create(&ping_t, &t_attr, &pinger, &pid);
 
	init_draw(0);
	return 0;
}
