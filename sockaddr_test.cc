#include <gtest/gtest.h>
extern "C" {
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <librtmp/log.h>
#include <arpa/inet.h>
}

namespace {
class SockTest : public testing::Test {
  protected:
  void SetUp() override {
    RTMP_LogSetLevel(RTMP_LOGALL);
  }
};

TEST_F(SockTest, addr) {
  // #include <sys/socket.h>
  struct sockaddr addr;
  EXPECT_EQ(sizeof(struct sockaddr), 16);
  EXPECT_EQ(sizeof(addr), 16);
}

TEST_F(SockTest, addrin) {
  // #include <netinet/in.h>
  struct sockaddr_in addr;
  EXPECT_EQ(sizeof(struct sockaddr_in), 16);
  EXPECT_EQ(sizeof(addr), 16);
}

/*
 * struct sockaddr_in {
 *   __uint8_t       sin_len; (1): aka: uint8_t
 *   sa_family_t     sin_family; (1) aka: uint8_t
 *   in_port_t       sin_port; (2) aka: uint16_t
 *   struct  in_addr sin_addr; (4) aka: uint32_t
 *   char            sin_zero[8];
 * };
*/
TEST_F(SockTest, memory_layout) {
  int expect_port = 80;
  // clang-format off
  struct sockaddr_in addr = {
    .sin_family = AF_INET, 
    .sin_port = htons(expect_port), 
    .sin_addr.s_addr = htonl(INADDR_ANY)
  };
  // clang-format on

  uint8_t *p = (uint8_t *)&addr;
  EXPECT_EQ(p[0], 0x00);
  EXPECT_EQ(p[1], 0x02);
  EXPECT_EQ(p[1], AF_INET);

  uint16_t port;
  *(&port + 0) = *(p + 3);
  *(&port + 1) = *(p + 2);
  EXPECT_EQ(port, expect_port);

  uint16_t port2 = ntohs(*(uint16_t *)(p + 2));
  EXPECT_EQ(port2, expect_port);
}

/*
 * uint32_t htonl(uint32_t hostlong);
 * hostlong(4) -> networklong(4)
 */
TEST_F(SockTest, htonl) {
  uint8_t expected[] = {0x00, 0x00, 0x00, 0x01};
  uint32_t nl = htonl(1);
  EXPECT_EQ(memcmp(expected, &nl, 4), 0);
}

/*
 * uint16_t ntohs(uint16_t netshort); 
 * networkshort(2) -> hostshort(2)
 */
TEST_F(SockTest, ntohs) {
  uint8_t expected[] = {0x50, 0x00};
  // 0x5000 => host memory layout: 00 50
  uint16_t hs = ntohs(0x5000);
  EXPECT_EQ(memcmp(expected, &hs, 2), 0);
}

TEST_F(SockTest, ntohs_with_bytes) {
  uint8_t expected[] = {0x50, 0x00};
  uint8_t src[] = {0x00, 0x50};

  uint16_t *nsptr = (uint16_t *)src;
  uint16_t result = ntohs(*nsptr);
  EXPECT_EQ(result, 0x50);
}

// #include <arpa/inet.h>
// inet_pton - convert IPv4 and IPv6 addresses from text to binary form
// int inet_pton(int af, const char *restrict src, void *restrict dst);
TEST_F(SockTest, inet_pton) {
  uint32_t addr;
  inet_pton(AF_INET, "192.168.0.1", &addr);

  uint8_t *p = (uint8_t *)&addr;
  EXPECT_EQ(p[0], 192);
  EXPECT_EQ(p[1], 168);
  EXPECT_EQ(p[2], 0);
  EXPECT_EQ(p[3], 1);

  // conv back
  char str[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &addr, str, INET_ADDRSTRLEN);
  strcmp("192.168.0.1", str);
}

TEST_F(SockTest, inet_ntop) {
  // clang-format off
  struct sockaddr_in addr = {
    .sin_family = AF_INET, 
    .sin_port = htons(80), 
    .sin_addr.s_addr = htonl(INADDR_ANY)
  };
  // clang-format on

  char str[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(addr.sin_addr), str, INET_ADDRSTRLEN);
  strcmp("0.0.0.0", str);
}

TEST_F(SockTest, inet_ntoa) {
  uint32_t addr;
  struct in_addr sa;
  sa.s_addr = inet_addr("127.0.0.1");
  char *str = inet_ntoa(sa);
  EXPECT_EQ(strcmp(str, "127.0.0.1"), 0);
}
} // namespace