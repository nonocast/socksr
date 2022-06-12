#include <gtest/gtest.h>
extern "C" {
#include <librtmp/log.h>
#include "socks.h"
}

namespace {
class SocksTest : public testing::Test {
  protected:
  void SetUp() override {
    RTMP_LogSetLevel(RTMP_LOGALL);
  }
};

TEST_F(SocksTest, make) {
  // 05 01 00 01 22 ce 27 99  00 50
  // 34.206.39.153:80
  uint8_t buffer[] = {0x05, 0x01, 0x00, 0x01, 0x22, 0xce, 0x27, 0x99, 0x00, 0x50};
  request_t request;
  memset(&request, 0x00, sizeof(request));
  make_request(&request, buffer, sizeof(buffer));
  EXPECT_EQ(request.ver, 5);
  EXPECT_EQ(request.cmd, 1);
  EXPECT_EQ(request.atype, 1);
  EXPECT_EQ(request.port, 80);
  // RTMP_LogHex(RTMP_LOGDEBUG, (uint8_t *)&request.addr, sizeof(struct sockaddr));
  // printf("%s\n", inet_ntoa(((struct sockaddr_in *)&request.addr)->sin_addr));
  EXPECT_EQ(strcmp(inet_ntoa(((struct sockaddr_in *)&request.addr)->sin_addr), "34.206.39.153"), 0);
  EXPECT_EQ(((struct sockaddr_in *)&request.addr)->sin_port, 80);
}
} // namespace