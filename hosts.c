#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "hostnr.h"
#include "hosts.h"

struct geneneric_network_host* hosts;
int HOSTC = 4;
void setup_one_host(int id, char* addr){
	hosts[id].ping_us = 0;
	hosts[id].last_seen = time(0);
	hosts[id].last_seq = 0;
	hosts[id].addr.sin_family = AF_INET;
	hosts[id].addr.sin_port = 0;
	inet_aton(addr, &hosts[id].addr.sin_addr);
}
int setup_hosts() {
	hosts = malloc(HOSTC*sizeof(struct geneneric_network_host));
	setup_one_host(HOST_ROUTER, "10.10.0.1");
	setup_one_host(HOST_SWITCH_SERV1, "10.10.0.11");
	setup_one_host(HOST_SWITCH_SERV2, "10.10.0.12");
	setup_one_host(HOST_SWITCH_SERV3, "10.10.0.13");
	return 0;
}
