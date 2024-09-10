#pragma once
#include <arpa/inet.h>
extern int HOSTC;
extern struct drawc* drawc;
extern int drawcc;
enum draw_t {
	DRAW_SECTION,
	DRAW_LSECTION,
	DRAW_BR,
	DRAW_HOST,
};
struct drawc {
	char* name;
	enum draw_t type;
};

struct geneneric_network_host {
	int ping_us;
	int last_seq;
	int last_seen;
	struct sockaddr_in addr;
};

extern struct geneneric_network_host* hosts;
int setup_hosts();
