CXX = g++
CXXFLAGS = -std=c++14

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

TARGETS = send_cs.out

.PHONY: all clean

all: $(TARGETS)

send_cs.out: send_cs.cpp
	$(CXX) $(CXXFLAGS) $< $(LIBS) -o $@

clean:
	rm -f $(TARGETS)