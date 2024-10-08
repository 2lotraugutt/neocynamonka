#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "setup.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "log.h"
#include "token.h"
#include "pipe.h"
#define ERREXT(x) {dprintf(2, "\033[31m%s\033[0m\n", x); exit(-1);}
#define SYNTAXERR(x) {dprintf(2, "\033[31mSyntax error in %s line %d (at \"\033[1m%s\033[0;31m\") (%s)\033[0m\n",cynamonka_config , yylineno, yytext, x); exit(-1);}


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

int HOSTC = 0;
#define MAX_HOSTS 512
// #define cynamonka_config "/etc/cynamonka.conf"
extern char* cynamonka_config;


extern int yylex();
extern void yyset_in();
extern int yylineno;
extern char* yytext;

extern bool quiet;


int setup() {
	hosts = malloc(MAX_HOSTS*sizeof(struct geneneric_network_host));
	drawc = malloc(MAX_HOSTS*sizeof(struct drawc));
	FILE* fp = fopen(cynamonka_config, "r");
	if (fp == NULL) ERREXT("Failed to open config file");
	yyset_in(fp);
	char buf[1024]; size_t buf_pos = 0;
	char* args[10]; int argc = 0;
	int ntoken, vtoken;
	ntoken = yylex();
	while(ntoken) {
		switch (ntoken){
			case PIPE:
				if (yylex() != ID) SYNTAXERR("Expected ID")
				setup_pipe(yytext);
				break;
			case QUIET:
				quiet = true; break;
			case SECTION:
				if (yylex() != ID) SYNTAXERR("Expected ID")
	 			drawc[drawcc].type = DRAW_SECTION; 
	 			drawc[drawcc++].name = strdup(yytext); 
	 			break;
			case LSECTION:
				if (yylex() != ID) SYNTAXERR("Expected ID")
	 			drawc[drawcc].type = DRAW_LSECTION; 
	 			drawc[drawcc++].name = strdup(yytext); 
	 			break;
			case HOST: drawc[drawcc].type = DRAW_HOST; goto host_common;
			case CHOST: drawc[drawcc].type = DRAW_CHOST; goto host_common;
			case OHOST: drawc[drawcc].type = DRAW_OHOST; goto host_common;
			host_common:
				if (yylex() != IPADDR) SYNTAXERR("Expected IPAddr")
	 			setup_one_host(HOSTC++, yytext);
				if (yylex() != ID) SYNTAXERR("Expected ID")
	 			drawc[drawcc++].name = strdup(yytext);
				break;
			case BR:
	 			drawc[drawcc].type = DRAW_BR; 
	 			drawc[drawcc++].name = NULL; 
	 			break;
			default:
			case UNEXPECTED:
				SYNTAXERR("Unexpected Identifier")

		}
		ntoken = yylex();
	}
	return 0;
}
