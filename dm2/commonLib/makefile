# デフォルトのインストール先
PREFIX ?= /usr/local
LIBDIR := $(PREFIX)/lib

#トレースログ機能(1:有効,0:無効) ※無効にするとACCEANヘッダからtracelogがなくなる
TRACELOG = 0

#LID毎優先処理機能(0:無効, 1:有効)
LID_PRIORITY = 0

COMPILER = g++
TARGET = dm2SocketInterface
CFLAGS = -DTRACELOG=$(TRACELOG) -DLID_PRIORITY=$(LID_PRIORITY) -std=c++11 -pthread -MMD -MP -Wall -O3 -fPIC -llog4cxx
INCLUDES = -I../csLib/include -I./include/dm2SocketInterface -I../secLib/include

OBJ_DIR = obj/dm2SocketInterface
SRC_DIR_CSLIB = ../csLib/source
SRC_DIR_IF = source/dm2SocketInterface
LIBDIR_LOCAL = lib

IF_SOURCES = \
	$(SRC_DIR_IF)/UdpReceiveInterface.cpp \
	$(SRC_DIR_IF)/UdpSendInterface.cpp

CSLIB_SOURCES = \
	$(SRC_DIR_CSLIB)/socket/Socket.cpp \
	$(SRC_DIR_CSLIB)/socket/SocketUdp.cpp \
	$(SRC_DIR_CSLIB)/socket/UdpMcNwClient.cpp \
	$(SRC_DIR_CSLIB)/socket/UdpMcNwServer.cpp \
	$(SRC_DIR_CSLIB)/socket/UdpNwClient.cpp \
	$(SRC_DIR_CSLIB)/socket/UdpNwServer.cpp \
	$(SRC_DIR_CSLIB)/socket/UdpProcClient.cpp \
	$(SRC_DIR_CSLIB)/socket/UdpProcServer.cpp \
	$(SRC_DIR_CSLIB)/socket/UdpServer.cpp \
	$(SRC_DIR_CSLIB)/socket/UdpClient.cpp \
	$(SRC_DIR_CSLIB)/socket/IUdpServer.cpp \
	$(SRC_DIR_CSLIB)/socket/IUdpClient.cpp \
	$(SRC_DIR_CSLIB)/container/Queue.cpp \
	$(SRC_DIR_CSLIB)/container/UnorderedMap.cpp \
	$(SRC_DIR_CSLIB)/container/Map.cpp \
	$(SRC_DIR_CSLIB)/util/Util.cpp

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(CSLIB_SOURCES) $(IF_SOURCES)))
DEPENDS = $(OBJS:.o=.d)

.PHONY: all clean install uninstall lib

all: $(TARGET)

$(TARGET): lib $(OBJS)
	$(COMPILER) $(CFLAGS) -shared $(OBJS) -o $(LIBDIR_LOCAL)/lib$(TARGET).so

$(OBJ_DIR)/%.o: $(SRC_DIR_CSLIB)/socket/%.cpp
	$(COMPILER) $(CFLAGS) -c $(INCLUDES) $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR_CSLIB)/container/%.cpp
	$(COMPILER) $(CFLAGS) -c $(INCLUDES) $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR_CSLIB)/util/%.cpp
	$(COMPILER) $(CFLAGS) -c $(INCLUDES) $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR_IF)/%.cpp
	$(COMPILER) $(CFLAGS) -c $(INCLUDES) $< -o $@

lib:
	install -d $(LIBDIR_LOCAL) obj $(OBJ_DIR)

install:
	install -d $(LIBDIR)
	cp -p $(LIBDIR_LOCAL)/lib$(TARGET).so $(LIBDIR)/
	
uninstall:
	rm -rf $(LIBDIR)/lib$(TARGET).so

clean:
	rm -f $(OBJ_DIR)/*.o $(LIBDIR_LOCAL)/*.so $(DEPENDS)

-include $(DEPENDS)