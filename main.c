#include <pthread.h>
#include <stdio.h>
#include "draw.h"
#include "ping.h"
#include "hosts.h"
#include "log.h"
#include "pipe.h"

enum arg_type {
	OPT,
	CONF_FILE,
};

char* cynamonka_config = "/etc/cynamonka.conf";

int main(int argc, char** argv) {
	// init_log_fd();
	setup_pipe();
	bool quiet = false;
	enum arg_type next_arg = OPT;
	for (int i = 1; i<argc; i++)
		if(next_arg == OPT && argv[i][0]=='-')
			switch(argv[i][1]){
				case 'q': quiet = true;
				case 'c': next_arg = CONF_FILE; break;
			}
		else {
			switch(next_arg) {
				case CONF_FILE:
					cynamonka_config = argv[i]; break;
				case OPT:
				default: 
					dprintf(2, "\033[31mFailed to read comand line args\033[0m\n"); exit(-1);
			}
			next_arg = OPT;
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
