#include "ping.h"
#include "thread_configs.h"
#include "log.h"


unsigned short checksum(void *b, int len) 
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

void response(void *buf, int pid, unsigned long long ret) 
{
	pid = htons(pid);
	struct iphdr   *ip   = buf;
	struct icmphdr *icmp = buf + ip->ihl * 4;

	int hdr_size = sizeof(struct iphdr) + sizeof(struct icmphdr);
	unsigned long send_time_sec, send_time_nanosec;
	char *name = malloc(6);
	unsigned char ver;
	unsigned short hid;
	memcpy(name,               ((char *)buf) + hdr_size,      5);
	memcpy(&ver,               ((char *)buf) + hdr_size + 5,  1);
	memcpy(&hid,            ((char *)buf) + hdr_size + 6,  2);
	memcpy(&send_time_sec,     ((char *)buf) + hdr_size + 8,  4);
	memcpy(&send_time_nanosec, ((char *)buf) + hdr_size + 12, 4);
	for (int i = 0; i < 5; i++)
		name[i] = name[i] - '0';
	name[5] = '\0';

	if (strcmp(name, "PINGD") != 0) { return; }
	send_time_sec     = ntohl(send_time_sec);
	send_time_nanosec = ntohl(send_time_nanosec);
	if (hid>=0 && hid<HOSTC) {
		hosts[hid].ping_us = ret - ((unsigned long long) send_time_sec * 1000000 + (unsigned long long) send_time_nanosec / 1000);
		hosts[hid].last_seen = time(0);
	}
	LOG("ping response from host nr %d\n", hid);
}

void* listener(void* pid_VP) 
{
	short pid = *(int*)pid_VP;
	int sd;
	struct sockaddr_in addr;
	unsigned char buf[1024];

	if ((sd = socket(PF_INET, SOCK_RAW|SOCK_NONBLOCK, IPPROTO_ICMP)) < 0 ) {
		exit(1);
	}
	struct epoll_event ev, events[MAX_EVENTS];
	int nfds, epollfd;
	struct timespec t;

	ev.events = EPOLLIN;
	if ((epollfd = epoll_create1(0)) == -1)
		fprintf(stderr, "epoll_create1");
	ev.data.fd = sd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sd, &ev) == -1)
		fprintf(stderr, "epoll_ctl sfd");
	for (;;) {
		if ((nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1)) == -1)
			fprintf(stderr, "epoll_wait");

		for (int n = 0; n < nfds; ++n) {
			if (events[n].data.fd == sd) {
				int bytes;
				unsigned int len = sizeof(addr);

				memset(buf, 0, sizeof(buf));
				clock_gettime(CLOCK_REALTIME, &t);
				unsigned long long ret = t.tv_sec * 1000000 + t.tv_nsec / 1000;
				bytes = recvfrom(sd, buf, sizeof(buf), 0, (struct sockaddr*) &addr, &len);
				if (bytes > 0)
					response(buf, pid, ret);
				else
					perror("recvfrom");
			}
		}
	}
	exit(0);
}

void ping(struct sockaddr_in *addr, int pid, unsigned short cnt, unsigned short hostid) 
{
	const int ttl  = 61;
	int sd;
	struct packet pckt;

	if ((sd = socket(PF_INET, SOCK_RAW|SOCK_NONBLOCK, IPPROTO_ICMP)) < 0) {
		perror("socket");
		return;
	}
	if (setsockopt(sd, SOL_IP, IP_TTL, &ttl, sizeof(ttl)) != 0)
		perror("Set TTL option");
	if (fcntl(sd, F_SETFL, O_NONBLOCK) != 0 )
		perror("Request nonblocking I/O");

	memset(&pckt, 0, sizeof(pckt));
	pckt.hdr.type       = ICMP_ECHO;
	pckt.hdr.un.echo.id = pid;

	struct timespec t;
	clock_gettime(CLOCK_REALTIME, &t);
	unsigned long org_s  = htonl(t.tv_sec);
	unsigned long org_ns = htonl(t.tv_nsec);
	const unsigned char ver = 1;
	char *name = "PINGD";
	unsigned char *header = malloc(5);
	for (unsigned int i = 0; i < strlen(name); i++)
		header[i] = name[i] + '0';

	memcpy(pckt.msg,      header,  5);
	memcpy(pckt.msg + 5,  &ver,    1);
	memcpy(pckt.msg + 6,  &hostid, 2);
	memcpy(pckt.msg + 8,  &org_s,  4);
	memcpy(pckt.msg + 12, &org_ns, 4);

	pckt.hdr.un.echo.sequence = htons(cnt);
	pckt.hdr.checksum = checksum(&pckt, sizeof(pckt));

	if (sendto(sd, &pckt, sizeof(pckt), 0, (struct sockaddr*)addr, sizeof(*addr)) <= 0 )
		perror("sendto");
}
void* pinger(void* VP) {
	int pid = *(int*)VP;
	for(;;)  {
		for(int i = 0; i<HOSTC; i++) {
			ping(&hosts[i].addr,pid, hosts[i].last_seq++, i);
		}
		sleep(1);
	}
	return NULL;
}
