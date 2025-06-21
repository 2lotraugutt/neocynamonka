#pragma  once
#include <arpa/inet.h>
#include <pthread.h>

#include "token.h"

struct drawcmd {
  enum token type;
  uint16_t displays;
  void* data;
};

struct host {
  pthread_mutex_t lock;
  char* name;
  int ping_us;
  int last_seq;
  int last_seen;
  struct sockaddr_in addr;
};
