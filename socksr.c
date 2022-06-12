#include "socksr.h"

int main(void) {
  RTMP_LogSetLevel(RTMP_LOGDEBUG);
  uint16_t port = 1080;
  struct sockaddr_in name = {.sin_family = AF_INET, .sin_port = htons(port), .sin_addr.s_addr = htonl(INADDR_ANY)};
  int on = 1;

  int httpd = socket(PF_INET, SOCK_STREAM, 0);
  if (httpd == -1)
    error_die("socket");
  if (setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
    error_die("setsocket failed");
  }
  if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0)
    error_die("bind");
  if (listen(httpd, 5) < 0)
    error_die("listen");

  printf("server start at %d\n", port);

  int client = -1;
  struct sockaddr_in client_name;
  socklen_t client_name_len = sizeof(client_name);
  pthread_t newthread;

  while (true) {
    int client = accept(httpd, (struct sockaddr *)&client_name, &client_name_len);
    if (client == -1)
      error_die("accept");
    if (pthread_create(&newthread, NULL, (void *)accept_request, (void *)(intptr_t)client) != 0)
      perror("pthread_create");
  }

  close(httpd);
  return 0;
}

void error_die(const char *sc) {
  perror("error: ");
  exit(1);
}

void accept_request(void *arg) {
  int client = (intptr_t)arg;
  dump_peer(client);

  size_t readlen;
  uint8_t buffer[4096];

  // phase 1: negotiation
  readlen = read(client, buffer, sizeof(buffer));
  RTMP_LogHex(RTMP_LOGDEBUG, (const uint8_t *)buffer, readlen);
  write(client, "\x05\x00", 2);

  // phase 2.1: requests
  readlen = read(client, buffer, sizeof(buffer));
  request_t request;

  printf("@ make_request\n");
  make_request(&request, buffer, readlen);
  dump_request(&request);

  // connect
  printf("@ connect\n");
  int dst = socket(AF_INET, SOCK_STREAM, 0);
  if (dst == -1) {
    printf("socket failed.\n");
  }

  RTMP_LogHex(RTMP_LOGDEBUG, (const uint8_t *)&request.addr, sizeof(struct sockaddr));

  struct sockaddr_in sa;
  sa.sin_addr.s_addr = inet_addr("127.0.0.1");
  sa.sin_family = AF_INET;
  sa.sin_port = htons(80);
  if (connect(dst, &request.addr, sizeof(struct sockaddr)) < 0) {
    error_die("connect");
  }

  printf("@ connect ok\n");
  uint8_t response[] = {0x05, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  write(client, response, sizeof(response));

  while ((readlen = read(client, buffer, sizeof(buffer))) > 0) {
    RTMP_LogHexString(RTMP_LOGDEBUG, buffer, readlen);
    write(dst, buffer, readlen);
    readlen = read(dst, buffer, sizeof(buffer));
    write(client, buffer, readlen);
  }

  close(client);
}

void dump_peer(intptr_t sock) {
  struct sockaddr_in addr;
  socklen_t addrlen = sizeof(addr);
  int rc = getpeername(sock, (struct sockaddr *)&addr, &addrlen);
  if (rc == -1) {
    printf("getperrname failed");
  }
  printf("client: %s:%d\n", inet_ntoa(addr.sin_addr), (int)ntohs(addr.sin_port));
}