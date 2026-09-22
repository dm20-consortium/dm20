MODULE_NAME = dm2is
TARGET    = ./bin/$(MODULE_NAME)

USE_UDP_DMI = 0
USE_ROS2DMI = 0
USE_KOITO_DMI = 0
USE_ASMOBI_DMI = 0
ROS2_WORKSPACE_DIR = "\"/home/dm2/work/v080_ws\""

# デフォルトのインストール先
PREFIX ?= /usr/local
BINDIR     := $(PREFIX)/bin
COMPLETIONS_DIR = /usr/share/bash-completion/completions

#MEASURE_MODE(0[default]:Disable, 1:Enable)
MEASURE_MODE  = 0

COMPILER = g++

# C_PLUSのバージョン設定, PROTOBUF関連の設定
include ../common.mk

USER_HOME := $(shell eval echo ~$(SUDO_USER))
#DEBUGS = -g -O0
CFLAGS    = -DMEASURE_MODE=${MEASURE_MODE} -DCOMPILER_SWITCH=${COMPILER_SWITCH} -DUSE_UDP_DMI=${USE_UDP_DMI} -DUSE_ROS2DMI=${USE_ROS2DMI} -DROS2_WORKSPACE_DIR=$(ROS2_WORKSPACE_DIR) -DUSE_KOITO_DMI=${USE_KOITO_DMI} -DUSE_ASMOBI_DMI=${USE_ASMOBI_DMI} -std=${CPLUS_VER} $(DEBUGS) -MMD -MP -Wall -O3 -Wextra -Winit-self -Wno-missing-field-initializers -Wno-unused-parameter -Wno-shift-count-overflow

INCLUDE   = -I./include -I./../csLib/include -I../commonLib/include/dm2SocketInterface -I../tupleLib/include -I../proto/is/include -I../proto/api/include -I/usr/local/include/is/proto $(PROTOBUF_CFLAGS)

LDFLAGS   = -L../commonLib/lib -L../tupleLib/lib -L../proto/is/lib -L../proto/api/lib -L../secLib/lib $(PROTOBUF_LDFLAGS)
LIBS      = -lpthread -ldl -lpqxx -lpq -lxerces-c -ldm2SocketInterface -llog4cxx -lgeos_c -luuid -lssl -lcrypto -lm -ltuple -ldm2proto_is -ldm2proto_api -lz -lzstd -lyaml-cpp -lrt -ldm2security $(PROTOBUF_LIBS)
SRCDIR    = ./source

ifeq "$(strip $(SRCDIR))" ""
  SRCDIR  = .
endif
SOURCES   = $(wildcard $(SRCDIR)/*.cpp)
OBJDIR    = ./obj
ifeq "$(strip $(OBJDIR))" ""
  OBJDIR  = .
endif
#OBJECTS   = $(addprefix $(OBJDIR)/, $(notdir $(PROTOS_OUTS:.pb.cc=.pb.o))) $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))
OBJECTS   = $(addprefix $(OBJDIR)/,) $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))
DEPENDS   = $(OBJECTS:.o=.d)

$(TARGET): bin $(OBJECTS)
	$(COMPILER) $(DEBUGS) -o $@ $(OBJECTS) $(LDFLAGS) $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

TUPLE_LIB = ../tupleLib/lib/libtuple.a

.PHONY: all clean install uninstall bin legacy_proto_clean

legacy_proto_clean:
	rm -f include/*.pb.h
	rm -f source/*.pb.cc
	rm -f obj/*.pb.d obj/*.pb.o

all: clean legacy_proto_clean $(TARGET)

bin:
	install -d bin obj

link: $(TUPLE_LIB)
	$(COMPILER) $(DEBUGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(LIBS)

install:
	install -d $(BINDIR)
	rm -f $(BINDIR)/$(MODULE_NAME)
	cp bin/$(MODULE_NAME) $(BINDIR)/
	@if [ "$(PREFIX)" = "/usr" ] || [ "$(PREFIX)" = "/usr/local" ]; then \
		cp completion/$(MODULE_NAME) $(COMPLETIONS_DIR)/; \
	fi
	
uninstall:
	rm -f $(BINDIR)/$(MODULE_NAME) /$(MODULE_NAME)
	@if [ "$(PREFIX)" = "/usr" ] || [ "$(PREFIX)" = "/usr/local" ]; then \
		rm -f $(COMPLETIONS_DIR)/$(MODULE_NAME); \
	fi
	
clean:
	-rm -f $(OBJECTS) $(DEPENDS) $(TARGET)

-include $(DEPENDS)
