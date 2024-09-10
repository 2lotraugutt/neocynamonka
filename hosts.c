#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "hostnr.h"
#include "hosts.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "log.h"

struct geneneric_network_host* hosts;
struct drawc* drawc;
int drawcc;
void setup_one_host(int id, char* addr){
	hosts[id].ping_us = 0;
	hosts[id].last_seen = time(0);
	hosts[id].last_seq = 0;
	hosts[id].addr.sin_family = AF_INET;
	hosts[id].addr.sin_port = 0;
	inet_aton(addr, &hosts[id].addr.sin_addr);
}
enum DECLARATION {
	HOST,
	SECTION,
	BR,
};

int HOSTC = 0;
#define MAX_HOSTS 512
#define cynamonka_config "/etc/cynamonka.conf"

int setup_hosts() {
	hosts = malloc(MAX_HOSTS*sizeof(struct geneneric_network_host));
	drawc = malloc(MAX_HOSTS*sizeof(struct drawc));
	int fd = open(cynamonka_config, O_RDONLY);
	char buf[1024]; size_t buf_pos = 0;
	char* args[10]; int argc = 0;
	int last_wws = 0;
	enum DECLARATION dec = 0;
	while(read(fd,buf+(buf_pos),1)){
		if(buf[buf_pos] == ' ' || buf[buf_pos] == '\t' || buf[buf_pos] == '\n')
		{
			buf[buf_pos] = 0;
			if(last_wws) {continue; args[argc]++;};
			if(!argc) {
				LOG("BUF %s\n", buf);
				if(buf_pos == 4 && !memcmp("HOST", buf, 4))	dec = HOST;
				else if(buf_pos == 2 && !memcmp("BR", buf, 2))	dec = BR;
				else if(buf_pos == 7 && !memcmp("SECTION", buf, 7))	dec = SECTION;
				else {dec = 0 ; buf_pos=0;continue;}
			}

			if(argc == 2 && dec==HOST){
				LOG("Host %s %s\n", args[0], args[1]);
				setup_one_host(HOSTC++, args[0]);
				drawc[drawcc].type = DRAW_HOST;
				drawc[drawcc].name = strdup(args[1]);
				drawcc++; argc = 0; buf_pos = 0; continue;
			}
			else if(argc == 1 && dec==SECTION){
				LOG("Section %s\n", args[0]);
				LOG("SECTION\n");
				drawc[drawcc].type = DRAW_SECTION;
				drawc[drawcc].name = strdup(args[0]);
				drawcc++;
				argc = 0; buf_pos = 0; continue;
			}
			else if(argc == 0 && dec==BR){ 
				LOG("Break\n");
				LOG("BR\n");
				drawc[drawcc].type = DRAW_BR;
				drawc[drawcc].name = NULL;
				drawcc++;
				argc = 0; buf_pos = 0; continue;
			}
			else {
				args[argc++]=buf+buf_pos+1;
			}
		}else {
			if(last_wws) {
			}
		last_wws = 0;
		}
		buf_pos++;
	}
	return 0;
}
