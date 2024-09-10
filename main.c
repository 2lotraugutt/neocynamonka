#include <pthread.h>
#include <stdio.h>
#include "draw.h"
#include "ping.h"
#include "hosts.h"
#include "thread_configs.h"

int main() {
	setup_hosts();
	pthread_attr_t t_attr;
	pthread_attr_init(&t_attr);
	int pid = getpid();
	pthread_t ping_t, listen_t, draw_t;
	pthread_create(&draw_t, &t_attr, &init_draw, NULL);
	pthread_create(&listen_t, &t_attr, &listener, &pid);
	struct pinger_config pc;
	pthread_create(&ping_t, &t_attr, &pinger, &pid);
	pthread_join(draw_t, NULL);
	/* sleep(100000); */
}
