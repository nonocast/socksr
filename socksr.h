#ifndef SOCKSR_H
#define SOCKSR_H

#include <errno.h>
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
#include "socks.h"

void error_die(const char *);
void accept_request(void *arg);
void dump_peer(intptr_t sock);

#endif