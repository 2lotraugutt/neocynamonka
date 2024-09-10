#pragma once
#include <arpa/inet.h>
extern int HOSTC;

struct geneneric_network_host {
	int ping_us;
	int last_seq;
	int last_seen;
	struct sockaddr_in addr;
};

extern struct geneneric_network_host* hosts;
int setup_hosts();
