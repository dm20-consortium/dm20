MODULE_NAME = dm2is

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

#COMPILER_SWITCH(0[default]:g++, 1:Intel compiler)
COMPILER_SWITCH = 0
ifeq ($(COMPILER_SWITCH), 1)
  COMPILER = icc
endif
#/etc/os-releaseの下記XXの箇所を抽出し、メジャーバージョンを判定する
VERSION_ID := $(shell sed -n 's/^VERSION_ID="\([0-9]*\)\..*"/\1/p' /etc/os-release)
ifeq ($(VERSION_ID),18)
  COMPILER = g++-5
else
  COMPILER = g++
endif

ifeq ($(VERSION_ID),24)
  CPLUS_VER=c++17
else
  CPLUS_VER=c++14
endif

USER_HOME := $(shell eval echo ~$(SUDO_USER))
#DEBUGS = -g -O0
CFLAGS    = -DMEASURE_MODE=${MEASURE_MODE} -DCOMPILER_SWITCH=${COMPILER_SWITCH} -DUSE_UDP_DMI=${USE_UDP_DMI} -DUSE_ROS2DMI=${USE_ROS2DMI} -DROS2_WORKSPACE_DIR=$(ROS2_WORKSPACE_DIR) -DUSE_KOITO_DMI=${USE_KOITO_DMI} -DUSE_ASMOBI_DMI=${USE_ASMOBI_DMI} -DPQXX_VER=${PQXX_VER} -std=${CPLUS_VER} $(DEBUGS) -MMD -MP -Wall -O3 -Wextra -Winit-self -Wno-missing-field-initializers -Wno-unused-parameter -Wno-shift-count-overflow

LDFLAGS   = -L../commonLib/lib -L../tupleLib/lib -L../secLib/lib 
LIBS      = -lpthread -ldl -lpqxx -lpq -lxerces-c -ldm2SocketInterface -llog4cxx -lgeos_c -luuid -lssl -lcrypto -lm -ltuple -lz -lzstd -lyaml-cpp -lrt -ldm2security
TARGET    = ./bin/$(MODULE_NAME)
SRCDIR    = ./source

# ディレクトリパスに含まれる空白文字をコロンに変換する関数
# 使用方法）$(call space_to_colon, string)
#   注意）環境によっては、$(space)を半角スペースとして認識しない場合がある。
#       対処法）空文字を挟む事で半角スペースを認識させる
#            $(subst $() ,:,$(1))
space_to_colon = $(subst $() ,:,$(1))
space :=
space +=

# Protobuf定義ファイルのターゲット設定
PROTO_DIR     = ../proto
PROTO_OUT_DIR = ./include
PROTO_INCLUDE = $(wildcard $(PROTO_DIR)/*)
PROTO_INCLUDE_COLON = $(call space_to_colon,$(PROTO_INCLUDE))
PROTOS        = $(wildcard $(PROTO_DIR)/*/*.proto)
# 昇順（api, is）にソート
# 理由）is側のprotoファイルがapi側をインポートしているため
SORTED_PROTOS = $(sort $(PROTOS))
PROTOS_OUTS   = $(addprefix $(SRCDIR)/, $(notdir $(SORTED_PROTOS:.proto=.pb.cc)))

ifeq "$(strip $(SRCDIR))" ""
  SRCDIR  = .
endif
SOURCES   = $(wildcard $(SRCDIR)/*.cpp)
OBJDIR    = ./obj
ifeq "$(strip $(OBJDIR))" ""
  OBJDIR  = .
endif
OBJECTS   = $(addprefix $(OBJDIR)/, $(notdir $(PROTOS_OUTS:.pb.cc=.pb.o))) $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))
DEPENDS   = $(OBJECTS:.o=.d)

# Protobufの実行パス・ライブラリリンク方法の設定
PROTOLIB_OPT1 = 
PROTOLIB_OPT2 = -lprotobuf
PROTOC_PATH = protoc
PROTOC_INCLUDE_PATH = 

INCLUDE   = -I./include -I./../csLib/include -I../commonLib/include/dm2SocketInterface -I../tupleLib/include

$(TARGET): bin $(OBJECTS)
	$(COMPILER) $(DEBUGS) -o $@ $(PROTOLIB_OPT1) $(OBJECTS) $(LDFLAGS) $(LIBS) $(PROTOLIB_OPT2) 

$(OBJDIR)/%.pb.o: $(SRCDIR)/%.pb.cc
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

# 実行例) protoc -I=./proto/dm2:./proto/api --cpp_out=./include proto/api/dm2is.proto
$(SRCDIR)/%.pb.cc: $(PROTO_DIR)/*/%.proto
	$(PROTOC_PATH) -I=$(PROTO_INCLUDE_COLON) --cpp_out=$(PROTO_OUT_DIR) $<
	mv $(PROTO_OUT_DIR)/*.pb.cc $(SRCDIR)


.PHONY: all clean install uninstall bin proto
all: clean $(TARGET)

proto: $(PROTOS_OUTS)

bin:
	install -d bin obj

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
	-rm -f $(SRCDIR)/*.pb.cc $(PROTO_OUT_DIR)/*.pb.h $(OBJDIR)/*.pb.d $(OBJDIR)/*.pb.o

-include $(DEPENDS)
