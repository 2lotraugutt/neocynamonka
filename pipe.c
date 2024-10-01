#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "pipe.h"
#include <stdlib.h>
#include <time.h>
#include "setup.h"
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>

int pipefd = 0;

int setup_pipe(char* file) {
	if(pipefd) {dprintf(2, "\033[31mFailed to setup pipe at %s pipe is alredy present\033[0m\n", file); exit(-1);}
	pipefd = open(file, O_WRONLY|O_CREAT);
	return 0;
}

void* init_pipe(void* NONE) {// Start pipe update thread
	if (pipefd<=0) return NULL;
	char* host_type="normal";
	for(;;) {
		lseek(pipefd, 0, SEEK_SET);
		ftruncate(pipefd, 0);
		dprintf(pipefd, "{\n\t\"\": {\n");
		bool last_was_host = false;
		for (int i = 0; i<drawcc; i++){
			int hostnr = 0;
			int sec = time(0);
			switch( drawc[i].type ) {
				case DRAW_LSECTION:
				case DRAW_SECTION:
					dprintf(pipefd, "\n\t},\n\t\"%s\":{\n", drawc[i].name);
					last_was_host = false;
					break;
				case DRAW_HOST:
					host_type="normal"; 
					goto host_generic;
				case DRAW_OHOST:
					host_type="optional";
					goto host_generic;
				case DRAW_CHOST:
					host_type="critical";
					goto host_generic;
				host_generic:
					if(last_was_host) dprintf(pipefd, ",\n");
					struct geneneric_network_host host = hosts[hostnr++];
					dprintf(pipefd, "\t\t\"%s\":{\n", drawc[i].name);
					dprintf(pipefd, "\t\t\t\"%s\": %d,\n", "rtt", host.ping_us);
					dprintf(pipefd, "\t\t\t\"%s\": %d,\n", "last_seen", host.last_seen);
					dprintf(pipefd, "\t\t\t\"%s\": %d,\n", "last_seen_d", sec-host.last_seen);
					dprintf(pipefd, "\t\t\t\"%s\": \"%s\"\n", "type", host_type);
					dprintf(pipefd, "\t\t}");
					last_was_host = true;
					break;
				default:break;
			};
		}
			
		dprintf(pipefd, "\n\t}\n}\n");
		sleep(1);
	}
}
