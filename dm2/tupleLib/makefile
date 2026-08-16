# デフォルトのインストール先
PREFIX ?= /usr/local
INCLUDEDIR := $(PREFIX)/include/is
LIBDIR     := $(PREFIX)/lib

COMPILER = g++

# C_PLUSのバージョン設定, PROTOBUF関連の設定
include ../common.mk

CFLAGS    = -std=$(CPLUS_VER) -shared -fPIC -O3 -pthread
LDFLAGS   = $(PROTOBUF_LDFLAGS)
INCLUDE   = -I./include -I../proto/api/include -I../proto/is/include -I/usr/local/include/is $(PROTOBUF_CFLAGS)
SRCDIR    = ./source
OBJDIR    = ./obj
LIBDIR_LOCAL = ./lib
TARGET    = ./lib/libtuple.a

# ビルド対象ファイルと依存ファイル
SOURCES   = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS   = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))
DEPENDS   = $(OBJECTS:.o=.d)

.PHONY: all clean install uninstall lib
all: lib $(TARGET)

$(TARGET): $(OBJECTS)
	ar rcs $@ $(OBJECTS)
  
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

lib:
	install -d $(LIBDIR_LOCAL) $(OBJDIR)

install:
	install -d $(INCLUDEDIR) $(LIBDIR)
	cp -p include/is/* $(INCLUDEDIR)/
	rm -f $(LIBDIR)/libtuple.*
	cp -p $(TARGET) $(LIBDIR)/
	
uninstall:
	rm -rf $(INCLUDEDIR)
	rm -f $(LIBDIR)/libtuple.*

clean:
	-rm -f $(OBJECTS) $(DEPENDS) $(TARGET)

-include $(DEPENDS)
