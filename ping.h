#include <netinet/ip_icmp.h>
#include <time.h>
#include <stdint.h>

struct packet {
  struct icmphdr hdr;
  uint32_t hostid;
  struct timespec sendtime;
};

void* pinger(void*);
void* listener(void*);
