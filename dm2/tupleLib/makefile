# デフォルトのインストール先
PREFIX ?= /usr/local
INCLUDEDIR := $(PREFIX)/include/is
LIBDIR     := $(PREFIX)/lib

COMPILER = g++

#/etc/os-releaseからメジャーバージョンを判定する
UBUNTU_VERSION := $(shell . /etc/os-release && echo $$VERSION_ID | cut -d. -f1)

CPLUS_VER=c++17
ifeq ($(shell [ $(UBUNTU_VERSION) -le 22 ] && echo yes),yes)
  ROS_DISTRO := $(shell echo $$ROS_DISTRO)
  ifneq ($(ROS_DISTRO),humble)
    CPLUS_VER=c++14
  endif
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
