#include <pthread.h>
#include <stdio.h>
#include "draw.h"
#include "ping.h"

int main() {
	pthread_attr_t t_attr;
	pthread_attr_init(&t_attr);
	pthread_t ping_t, listen_t, draw_t;
	/*pthread_create(&draw_t, &t_attr, &init_draw, NULL);*/
	struct listener_config lc;
	int pid = getpid();
	printf("PID: %d\n", pid);
	pthread_create(&listen_t, &t_attr, &listener, &pid);
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	inet_aton("10.10.0.1", &(addr.sin_addr));
	addr.sin_port = htons(0);
	ping(&addr, pid, 0, 0);
	/*pthread_join(draw_t, NULL);*/
	sleep(100);
}
