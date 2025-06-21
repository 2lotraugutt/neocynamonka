#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "glob.h"
#include "setup.h"
#include "hosts.h"
#include "vector.h"
#include "token.h"
#include "log.h"

int parse_cmdarg( int argc, char** argv) {
	if (argc >= 2)
		config_file = argv[1];
  return 0;  
}

int parse_config( char* loc_config_file ) {

  if(vector_init_sized(1<<10, &drawcmd_vec)) CRITICAL_ERROR("Out of memory");
  if(vector_init_sized(1<<10, &hosts_vec)) CRITICAL_ERROR("Out of memory");

  FILE* config_file_fp = fopen(loc_config_file, "rb");
	if (config_file_fp == NULL) CRITICAL_ERROR("Failed to open config file");
	yyset_in(config_file_fp);

	pthread_mutexattr_t mutex_attr;
	if(pthread_mutexattr_init(&mutex_attr)) CRITICAL_ERROR("Failed to initialize mutex attributes");

		uint16_t disp = 1;
		for(int ntoken = yylex(); ntoken; ntoken = yylex()) {
	  struct drawcmd* dc = malloc(sizeof(struct drawcmd));
	  if(!dc) CRITICAL_ERROR("Out of memory");
	  dc->type = ntoken;
	  dc->displays = disp;

		switch (ntoken){
			case SECTION:
			case LSECTION:
				if (yylex() != ID) LEX_SYNTAXERR("Expected ID")
				dc->data = strdup(yytext);
				if(!dc->data) CRITICAL_ERROR("Out of memory");
	 			break;

 			case HOST:
		  case OHOST:
	    case CHOST:
			{
				struct host* host =  malloc(sizeof(struct host));
				if(!host) CRITICAL_ERROR("Out of memory");
  		  dc->data = host;
        host->last_seen = time(0);
        host->last_seq = 0;
        host->ping_us = 0;

  		  if(pthread_mutex_init(&host->lock, &mutex_attr)) CRITICAL_ERROR("Failed to initialize mutex");
  		  
				if (yylex() != IPADDR) LEX_SYNTAXERR("Expected ipv4 address")
				inet_aton(yytext, &host->addr.sin_addr);
        host->addr.sin_family=AF_INET;
        host->addr.sin_port = 0;

				if (yylex() != ID) LEX_SYNTAXERR("Expected ID")
        host->name = strdup(yytext);
      	if(!host->name) CRITICAL_ERROR("Out of memory");
  		  if(vector_append(host, &hosts_vec)) CRITICAL_ERROR("Out of memory");

				break;
			}
			case BR:
	 			break;

	 		case DISP:
	 			if (yylex() != ID) LEX_SYNTAXERR("Expected ID after DISP")
	 			disp = 0;
	 			for(char* ptr = yytext; *ptr; ++ptr)
	 				if(*ptr >= '1' && *ptr <= '9')
	 					disp |= 1<<(*ptr - '0')>>1;
	 				else
		 				LEX_SYNTAXERR("Allowed 1-9 after DISP");
	 			continue;
  		default:
			case UNEXPECTED:
				LEX_SYNTAXERR("Unexpected Token")

		}
		if(vector_append(dc, &drawcmd_vec)) CRITICAL_ERROR("Out of memory");
	}
  return 0;  
}
