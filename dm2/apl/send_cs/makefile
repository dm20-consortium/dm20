CXX = g++

# C_PLUSのバージョン設定, PROTOBUF関連の設定
include ../../common.mk

CFLAGS    = -std=${CPLUS_VER} 
LIBS = \
	-lxerces-c \
	-ldmclient \
	-ltuple \
	-ldm2proto_is \
	-ldm2proto_api \
	-lprotobuf \
	-lcrypto \
	-lssl \
	-lz \
	-lzstd \
	-lgeos_c \
	-pthread

TARGETS = recv_test_stream.out send_test_stream.out

.PHONY: all clean

all: $(TARGETS)

recv_test_stream.out: recv_test_stream.cpp
	$(CXX) $(CXXFLAGS) $< $(LIBS) -o $@

send_test_stream.out: send_test_stream.cpp
	$(CXX) $(CXXFLAGS) $< $(LIBS) -o $@

clean:
	rm -f $(TARGETS)