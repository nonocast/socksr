CC=clang -g
CXX=clang++
COMPILE.c = $(CC) $(CFLAGS) $(TARGET_ARCH) -c 
ARCH=-arch x86_64
CFLAGS=$(ARCH) -Wall -Wno-unused
CPPFLAGS=$(ARCH) -std=c++11
LDFLAGS=-lrtmp
OBJECTS=socks.o
TESTS=sockaddr_test.o socks_test.o
PROG=socksr test

all: $(PROG)

run: socksr
	@./socksr

t: run.test

run.test: test
	@./test

test: $(OBJECTS) $(TESTS)
	$(CXX) $(ARCH) `pkg-config --cflags --libs gtest` -L. -lgtest_main -lrtmp -o test $(OBJECTS) $(TESTS)

socksr: $(OBJECTS) socksr.o
socksr.o: socksr.c socksr.h Makefile
socks.o: socks.c socks.h Makefile

sockaddr_test.o: sockaddr_test.cc Makefile
socks_test.o: socks_test.cc Makefile

clean:
	@rm -f $(OBJECTS) $(PROG)