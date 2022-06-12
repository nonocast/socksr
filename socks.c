#include "socks.h"

void make_request(request_t *request, const uint8_t *buffer, size_t len) {
  request->ver = buffer[0];
  request->cmd = buffer[1];
  request->atype = buffer[3];
  request->port = ntohs(*(uint16_t *)(buffer + len - 2));
  if (request->atype == 1) {
    request->addr.sa_family = AF_INET;

    struct sockaddr_in *inp = (struct sockaddr_in *)&request->addr;
    inp->sin_port = htons(request->port);

    uint8_t *p = (uint8_t *)&request->addr;
    memcpy(p + 4, buffer + 4, 4);
  }
}

void dump_request(request_t *request) {
  printf("# request ");
  switch (request->cmd) {
  case 0x01:
    printf("CONNECT ");
    break;
  case 0x02:
    printf("BIND ");
    break;
  case 0x03:
    printf("UDP ");
    break;
  default:
    printf("UNKNOWN ");
    break;
  }

  switch (request->atype) {
  case 0x01: {
    struct sockaddr_in *p = (struct sockaddr_in *)&request->addr;
    printf("%s:%d", inet_ntoa(p->sin_addr), p->sin_port);
    break;
  }
  case 0x03:
    break;
  case 0x04:
    break;
  default:
    printf("UNKNOWN ");
    break;
  }
  printf("\n");
}