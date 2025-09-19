#include <asm-generic/errno.h>
#include <netinet/ip.h>
#include <stdint.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>

#include <fcntl.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>

#include "log.h"
#include "vector.h"
#include "glob.h"
#include "ping.h"

unsigned short icmp_checksum(void *b, int len) 
{
	unsigned short *buf = b;
	unsigned int sum=0;
	unsigned short result;

	for (sum = 0; len > 1; len -= 2 )
		sum += *buf++;
	if (len == 1 )
		sum += *(unsigned char*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}

#define TOUS(time) ((time)->tv_sec * 10000000 + (time)->tv_nsec/1000)
int handle_ping_packet(const struct packet *pkt, uint16_t pid, const struct timespec *time, struct sockaddr_in* addr) {

	if(pkt->hostid >= hosts_vec.len) return -1;
	struct host *host = hosts_vec.elems[pkt->hostid];

	if(pkt->hdr.un.echo.id != pid) return -2;
	if(memcmp(&host->addr, addr, sizeof(struct sockaddr_in))) return -3;
	if(host->last_seq != ntohs(pkt->hdr.un.echo.sequence)+1) return -4;

	host->last_seen = time->tv_sec;
	host->ping_us = TOUS(time) - TOUS(&pkt->sendtime);

	return 0;
}


#define MAX_EVENTS 256
void* listener(void* pid_VP) 
{
	uint16_t pid = *(uint16_t*)pid_VP;
	int epollfd, sock;
	struct sockaddr_in addr;
	size_t buf_len = sizeof(struct iphdr) + sizeof(struct packet);
	uint8_t buf[buf_len];

	CRIT_HANDLE((sock = socket(PF_INET, SOCK_RAW|SOCK_NONBLOCK, IPPROTO_ICMP)) < 0 , "socket")

	struct epoll_event ev, events[MAX_EVENTS];
	struct timespec time;

	ev.events = EPOLLIN;
	ev.data.fd = sock;

	CRIT_HANDLE((epollfd = epoll_create1(0)) == -1, "epoll_create1")
	CRIT_HANDLE(epoll_ctl(epollfd, EPOLL_CTL_ADD, sock, &ev) == -1, "epoll_ctl sfd")

	for (size_t recived;;) {
		CRIT_HANDLE((recived = epoll_wait(epollfd, events, MAX_EVENTS, -1)) == -1, "epoll_wait")
		for (int n = 0; n < recived; ++n)
			if (events[n].data.fd == sock) {
				clock_gettime(CLOCK_REALTIME, &time);
				uint32_t addr_len = sizeof(struct iphdr);
				int rb = recvfrom(sock, buf, buf_len, 0, (struct sockaddr*) &addr, &addr_len);
				if (rb == buf_len) {
					handle_ping_packet((struct packet*)(buf+sizeof(struct iphdr)), pid, &time, &addr);
				}
			}
			
	}
	return 0;
}

int send_ping_packet(struct sockaddr_in *addr, unsigned short seq, unsigned short hostid, int pid) 
{
	const int ttl  = 64;
	int sd;
	struct packet packet;

	CRIT_HANDLE((sd = socket(PF_INET, SOCK_RAW|SOCK_NONBLOCK, IPPROTO_ICMP)) < 0, "socket");
	CRIT_HANDLE(setsockopt(sd, SOL_IP, IP_TTL, &ttl, sizeof(ttl)) , "setsockopt");
	fcntl(sd, F_SETFL, O_NONBLOCK);
	memset(&packet, 0, sizeof(packet));
	
	CRIT_HANDLE(clock_gettime(CLOCK_REALTIME, &packet.sendtime) , "clock_gettime");
	packet.hostid = hostid;

	packet.hdr.type             = ICMP_ECHO;
	packet.hdr.un.echo.sequence = htons(seq);
	packet.hdr.un.echo.id       = pid;
	packet.hdr.checksum         = icmp_checksum(&packet, sizeof(packet));

	CRIT_HANDLE(sendto(sd, &packet, sizeof(packet), 0, (struct sockaddr*)addr, sizeof(*addr)) == -1 && errno!=ENETUNREACH, "sendto");

	close(sd);
	return 0;
}

void* pinger(void* VP) {
	uint16_t pid = *(uint16_t*)VP;
	for(;;)  {
		for(uint16_t i = 0; i<vector_len(&hosts_vec); i++) {
		  struct host *host = vector_get(i, &hosts_vec);
			send_ping_packet(&host->addr, host->last_seq++, i, pid);
		}
		sleep(1);
	}
	return NULL;
}



