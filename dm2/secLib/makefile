# デフォルトのインストール先
PREFIX ?= /usr/local
INSTALL_LIBDIR := $(PREFIX)/lib

TRACELOG     = 0
LID_PRIORITY = 0
ARCH         := $(shell uname -m)


COMPILER  = g++
CPLUS_VER = c++11

TARGETS   = ./lib/libdm2security.a 
CFLAGS    = -DTRACELOG=${TRACELOG} -DLID_PRIORITY=${LID_PRIORITY} -c -std=$(CPLUS_VER) -pthread -Wall -O3
INCLUDE   = -I./include
SRCDIR    = ./source
OBJDIR    = ./obj

# === ファイル ===
SOURCES   = $(wildcard $(SRCDIR)/*.cpp)
OBJS      = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))

.PHONY: all clean install uninstall lib
# === デフォルトターゲット ===
all: lib $(TARGETS)

# === .a ファイル生成 ===
$(TARGETS): $(OBJS)
	ar rcs $@ $(OBJS)

# === オブジェクトファイル生成 ===
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

# === ディレクトリ作成 ===
lib:
	install -d lib obj 

install:
	install -d $(INSTALL_LIBDIR)
	rm -f $(INSTALL_LIBDIR)/libdm2security.a
	cp lib/libdm2security.a $(INSTALL_LIBDIR)

uninstall:
	rm -f $(INSTALL_LIBDIR)/libdm2security.a

# === クリーンアップ ===
clean:
	-rm -f obj/*.o 
	-rm -f lib/*.a 
