# デフォルトのインストール先
PREFIX ?= /usr/local
BINDIR     := $(PREFIX)/bin
COMPLETIONS_DIR = /usr/share/bash-completion/completions

#/etc/os-releaseの下記XXの箇所を抽出し、メジャーバージョンを判定する
VERSION_ID := $(shell sed -n 's/^VERSION_ID="\([0-9]*\)\..*"/\1/p' /etc/os-release)
ifeq ($(VERSION_ID),24)
  CPLUS_VER    = c++17
else
  CPLUS_VER    = c++14
endif
#ディレクトリ名を実行ファイルとする
MODULE_NAME = $(shell basename `readlink -f .`)
TARGET    = ./bin/$(MODULE_NAME)

COMPILER  = g++
SRCDIR    = ./source
LIBS      = -lxerces-c -ltuple -ldmclient -lcrypto -lssl -lz -lzstd -lgeos_c -ldm2SocketInterface -ldm2security
CFLAGS    = -std=${CPLUS_VER} `pkg-config --cflags libpqxx` -pthread -Wall
INCLUDE   = -I./include -I../../tupleLib/include -I../../dmClientLib/include -I../../csLib/include -I../../commonLib/include/dm2SocketInterface
LDFLAGS   = -L../../tupleLib/lib -L../../dmClientLib/lib -L../../commonLib/lib -L../../secLib/lib 
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
