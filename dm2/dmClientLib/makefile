# デフォルトのインストール先
PREFIX ?= /usr/local
INCLUDEDIR := $(PREFIX)/include/is
LIBDIR     := $(PREFIX)/lib

#DEBUG_MODE(0:Disable, 1:Enable)
DEBUG = 0
#MEASURE_MODE(0:Disable, 1:Enable)
MEASURE_MODE = 0
#COMPRESS_MODE(0:Disable, 1:Enable)
COMPRESS_MODE = 0

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

CFLAGS    = -DDEBUG=${DEBUG} -DMEASURE_MODE=${MEASURE_MODE} -DCOMPRESS_MODE=${COMPRESS_MODE} -std=${CPLUS_VER} -shared -fPIC -O3 -pthread
LDFLAGS   = -L../tupleLib/lib
LIBS      = -lpthread -ltuple -lgeos_c -lxerces-c -lcrypto -lssl
INCLUDE   = -I./include -I../tupleLib/include -I../proto/is/include -I../proto/api/include -I/usr/local/include/is/proto
TARGET    = ./lib/libdmclient.a
SRCDIR    = ./source
ifeq "$(strip $(SRCDIR))" ""
  SRCDIR  = .
endif
SOURCES   = $(wildcard $(SRCDIR)/*.cpp)
OBJDIR    = ./obj
ifeq "$(strip $(OBJDIR))" ""
  OBJDIR  = .
endif
OBJECTS   = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))
DEPENDS   = $(OBJECTS:.o=.d)

$(TARGET): lib $(OBJECTS) 
	ar rcs lib/libdmclient.a $(OBJECTS)
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $< $(LIBS) $(LDFLAGS)
	
.PHONY: all clean install uninstall lib
all: clean $(TARGET)

lib:
	install -d lib obj

install:
	install -d $(INCLUDEDIR) $(LIBDIR)
	cp -p include/is/* $(INCLUDEDIR)/
	rm -f $(LIBDIR)/libdmclient.*
	cp -p $(TARGET) $(LIBDIR)/
	
uninstall:
	rm -rf $(INCLUDEDIR)
	rm -f $(LIBDIR)/libdmclient.*

clean:
	-rm -f $(OBJECTS) $(DEPENDS) lib/libdmclient.a

-include $(DEPENDS)
