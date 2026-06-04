# デフォルトのインストール先
PREFIX ?= /usr/local
INSTALL_LIBDIR := $(PREFIX)/lib

TRACELOG     = 0
LID_PRIORITY = 0
ARCH         := $(shell uname -m)


COMPILER  = g++
CPLUS_VER = c++11

TARGETS   = libdm2container.a libdm2util.a libdm2socket.a
CFLAGS    = -DTRACELOG=${TRACELOG} -DLID_PRIORITY=${LID_PRIORITY} -c -std=$(CPLUS_VER) -pthread -MMD -MP -Wall -O3
LDFLAGS   = -L$(LIB_DIR) -L$(CURDIR)/sslLib
INCLUDE   = -I./include -I../secLib/include

# === ファイル ===
CONTAINER_OBJS = obj/container/Queue.o obj/container/UnorderedMap.o obj/container/Map.o
UTIL_OBJS      = obj/util/Util.o
SOCKET_OBJS    = obj/socket/Socket.o obj/socket/SocketUdp.o obj/socket/SocketDTLS.o \
                 obj/socket/UdpMcNwClient.o obj/socket/UdpMcNwServer.o \
                 obj/socket/DtlsNwClient.o obj/socket/DtlsNwServer.o \
                 obj/socket/UdpNwClient.o obj/socket/UdpNwServer.o \
                 obj/socket/UdpProcClient.o obj/socket/UdpProcServer.o \
                 obj/socket/UdpClient.o obj/socket/UdpServer.o \
                 obj/socket/IUdpClient.o obj/socket/IUdpServer.o
DEPENDS = $(CONTAINER_OBJS:.o=.d) $(UTIL_OBJS:.o=.d) $(SOCKET_OBJS:.o=.d)

.PHONY: all clean install uninstall lib
# === デフォルトターゲット ===
all: lib $(TARGETS)

# === .a ファイル生成 ===
libdm2container.a: $(CONTAINER_OBJS)
	ar rsv lib/$@ $^

libdm2util.a: $(UTIL_OBJS)
	ar rsv lib/$@ $^

libdm2socket.a: $(SOCKET_OBJS)
	ar rsv lib/$@ $^

# === オブジェクトファイル生成 ===
obj/container/%.o: source/container/%.cpp include/*.h
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

obj/util/%.o: source/util/%.cpp include/*.h
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

obj/socket/%.o: source/socket/%.cpp include/*.h
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

# === ディレクトリ作成 ===
lib:
	install -d lib obj obj/container obj/socket obj/util

install:
	install -d $(INSTALL_LIBDIR)

uninstall:
	@:

# === クリーンアップ ===
clean:
	-rm -f obj/container/*.o obj/socket/*.o obj/util/*.o $(DEPENDS)
	-rm -f lib/*.a lib/*.so

-include $(DEPENDS)