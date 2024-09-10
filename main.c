#include <pthread.h>
#include <stdio.h>
#include "draw.h"
#include "ping.h"
#include "hosts.h"
#include "log.h"

int main(int argc, char** argv) {
	init_log_fd();
	bool quiet = false;
	for (int i = 0; i<argc; i++)
		if(argv[i][0]=='-')
			switch(argv[i][1]){
				case 'q': quiet = true;
			}

	setup_hosts();
	pthread_attr_t t_attr;
	pthread_attr_init(&t_attr);
	int pid = getpid();
	pthread_t ping_t, listen_t, draw_t;
	if(!quiet) pthread_create(&draw_t, &t_attr, &init_draw, NULL);
	pthread_create(&listen_t, &t_attr, &listener, &pid);
	pthread_create(&ping_t, &t_attr, &pinger, &pid);
	if(!quiet)pthread_join(draw_t, NULL);
	else pthread_join(ping_t, NULL);
}
