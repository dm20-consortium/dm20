# デフォルトのインストール先
PREFIX ?= /usr/local
INCLUDEDIR := $(PREFIX)/include/is
LIBDIR     := $(PREFIX)/lib

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

CFLAGS    = -std=$(CPLUS_VER) -shared -fPIC -O3 -pthread
LDFLAGS   =
INCLUDE   = -I./include
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