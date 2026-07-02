# デフォルトのインストール先
PREFIX ?= /usr/local
BINDIR     := $(PREFIX)/bin
COMPLETIONS_DIR = /usr/share/bash-completion/completions

#/etc/os-releaseからメジャーバージョンを判定する
UBUNTU_VERSION := $(shell . /etc/os-release && echo $$VERSION_ID | cut -d. -f1)

CPLUS_VER=c++17
ifeq ($(shell [ $(UBUNTU_VERSION) -le 22 ] && echo yes),yes)
  ROS_DISTRO := $(shell echo $$ROS_DISTRO)
  ifneq ($(ROS_DISTRO),humble)
    CPLUS_VER=c++14
  endif
endif

#ディレクトリ名を実行ファイルとする
MODULE_NAME = $(shell basename `readlink -f .`)
TARGET    = ./bin/$(MODULE_NAME)

COMPILER  = g++
SRCDIR    = ./source

INCLUDE   = -I./include -I../../dmClientLib/include -I../../tupleLib/include -I../../proto/is/include -I../../proto/api/include -I/usr/local/include/is/proto -I../../commonLib/include/dm2SocketInterface -I../../csLib/include

PROTOBUF_LIBS   := $(shell pkg-config --libs protobuf)
LDFLAGS   = -L../../dmClientLib/lib -L../../tupleLib/lib -L../../proto/is/lib -L../../proto/api/lib -L../../commonLib/lib -L../../secLib/lib 
LIBS      = -lxerces-c -ldmclient -ltuple -ldm2proto_is -ldm2proto_api -lcrypto -lssl -lz -lzstd -lgeos_c -ldm2SocketInterface -ldm2security $(PROTOBUF_LIBS)

CFLAGS    = -std=${CPLUS_VER} `pkg-config --cflags libpqxx` -pthread -Wall
OFLAGS    = -std=${CPLUS_VER} -pthread `pkg-config --libs libpqxx` `pkg-config --libs libpq`
SRCDIR    = ./source
SOURCES   = $(wildcard $(SRCDIR)/*.cpp)
OBJDIR    = ./obj
OBJECTS   = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))
DEPENDS   = $(OBJECTS:.o=.d)

${TARGET} : bin $(OBJECTS)
	$(COMPILER) -o $@ $(OBJECTS) ${OFLAGS} ${LDFLAGS} $(LIBS)

$(OBJDIR)/%.o: ${SRCDIR}/%.cpp
	$(COMPILER) -c ${CFLAGS} ${INCLUDE} $< -o $@ 

.PHONY: all clean install uninstall bin
all: clean $(TARGET)

bin:
	install -d bin obj

link: bin $(OBJECTS)
	$(COMPILER) -o $(TARGET) $(OBJECTS) $(OFLAGS) $(LDFLAGS) $(LIBS)

install:
	install -d $(BINDIR)
	rm -f $(BINDIR)/$(MODULE_NAME)
	cp bin/$(MODULE_NAME) $(BINDIR)/
	@if [ "$(PREFIX)" = "/usr" ] || [ "$(PREFIX)" = "/usr/local" ]; then \
		cp completion/$(MODULE_NAME) $(COMPLETIONS_DIR)/; \
	fi
	
uninstall:
	rm -f $(BINDIR)/$(MODULE_NAME)
	@if [ "$(PREFIX)" = "/usr" ] || [ "$(PREFIX)" = "/usr/local" ]; then \
		rm -f $(COMPLETIONS_DIR)/$(MODULE_NAME); \
	fi
	
clean:
	rm -f obj/*.o *.o
	rm -f ${DEPENDS} ${TARGET}
	
-include $(DEPENDS)
