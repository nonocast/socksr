#ifndef SOCKS_H
#define SOCKS_H

#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <librtmp/log.h>

typedef struct {
  uint8_t ver;
  uint8_t cmd;
  uint8_t atype;
  uint16_t port;
  char domain[255];
  struct sockaddr addr;
} request_t;

void make_request(request_t *request, const uint8_t *buffer, size_t len);
void dump_request(request_t *request);

#endif