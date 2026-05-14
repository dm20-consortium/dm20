
# デフォルトのインストール先
PREFIX ?= /usr/local
BINDIR     := $(PREFIX)/bin

#トレースログ機能(1:有効,0:無効) ※無効にするとACCEANヘッダからtracelogがなくなる
TRACELOG = 0

#CSで使用するキューサイズ設定
MAX_QUEUE_SIZE = 2129920000

#LID毎優先処理機能(0:無効, 1:有効)
LID_PRIORITY = 0

COMPILER=g++
SHELL=/bin/sh
CC  = $(COMPILER)
CFLAGS    = -std=c++11 -pthread -MMD -MP -Wall -O3 -DTRACELOG=${TRACELOG} -DMAX_QUEUE_SIZE=${MAX_QUEUE_SIZE} -DLID_PRIORITY=${LID_PRIORITY}
LDFLAGS   = -L../csLib/lib -Wl,-rpath /usr/local/lib -L../secLib/lib 
INCLUDES = -Iinclude -I../csLib/include -I../secLib/include
LIBS      = -ldm2socket -ldm2container -ldm2util -llog4cxx -lcrypto -lssl -ldm2security

BIN_DIR = bin
OBJ_DIR = obj

RECV_SRCS = NwRcv.cpp NwReceiver.cpp ProcSender.cpp
SEND_SRCS = ProcRcv.cpp ProcReceiver.cpp NwSender.cpp SidManagement.cpp

RECV_OBJS = $(addprefix $(OBJ_DIR)/, $(RECV_SRCS:.cpp=.o))
SEND_OBJS = $(addprefix $(OBJ_DIR)/, $(SEND_SRCS:.cpp=.o))
DEPENDS = $(RECV_OBJS:.o=.d) $(SEND_OBJS:.o=.d)

.PHONY: all clean install uninstall bin

all: bin dm2cs_send dm2cs_recv

dm2cs_send: $(SEND_OBJS)
	$(COMPILER) $(CFLAGS) -o $(BIN_DIR)/$@ $^ $(LDFLAGS) $(LIBS)

dm2cs_recv: $(RECV_OBJS)
	$(COMPILER) $(CFLAGS) -o $(BIN_DIR)/$@ $^ $(LDFLAGS) $(LIBS)

$(OBJ_DIR)/%.o: source/ProcRcv/%.cpp
	$(COMPILER) -c $(CFLAGS) $(INCLUDES) $< -o $@

$(OBJ_DIR)/%.o: source/NwRcv/%.cpp
	$(COMPILER) -c $(CFLAGS) $(INCLUDES) $< -o $@

bin:
	install -d bin obj

install:
	install -d $(BINDIR)
	rm -f $(BINDIR)/dm2cs_send $(BINDIR)/dm2cs_recv
	cp bin/dm2cs_send bin/dm2cs_recv $(BINDIR)/
	
uninstall:
	rm -f $(BINDIR)/dm2cs_send $(BINDIR)/dm2cs_recv

clean:
	-rm -f obj/*.o bin/dm2cs_send bin/dm2cs_recv $(DEPENDS)
	
-include $(DEPENDS)
