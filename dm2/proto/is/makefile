# デフォルトのインストール先
PREFIX ?= /usr/local
INSTALL_INCLUDEDIR := $(PREFIX)/include/is
INSTALL_LIBDIR     := $(PREFIX)/lib

# build
INCDIR := include
SRCDIR := source
OBJDIR := obj
LIBDIR := lib

TARGET := $(LIBDIR)/libdm2proto_is.so
PROTO_PATH := --proto_path=. --proto_path=../api
DEPEND_PROTO := $(wildcard ../api/*.proto)
DEPEND_INCLUDE := -I../api/include

COMPILER := g++

#/etc/os-releaseからメジャーバージョンを判定する
UBUNTU_VERSION := $(shell . /etc/os-release && echo $$VERSION_ID | cut -d. -f1)

CPLUS_VER := c++17
ifeq ($(shell [ $(UBUNTU_VERSION) -le 22 ] && echo yes),yes)
  ROS_DISTRO := $(shell echo $$ROS_DISTRO)
  ifneq ($(ROS_DISTRO),humble)
    CPLUS_VER := c++14
  endif
endif

PROTOC := protoc
PROTOBUF_CFLAGS := $(shell pkg-config --cflags protobuf)
PROTOBUF_LIBS   := $(shell pkg-config --libs protobuf)

CXXFLAGS := -std=$(CPLUS_VER) -fPIC -Wall $(PROTOBUF_CFLAGS)

# source
PROTO_FILES := $(wildcard *.proto)
PB_NAMES := $(basename $(PROTO_FILES))
PB_CC := $(addprefix $(SRCDIR)/,$(addsuffix .pb.cc,$(PB_NAMES)))
PB_H  := $(addprefix $(INCDIR)/,$(addsuffix .pb.h,$(PB_NAMES)))
OBJS := $(addprefix $(OBJDIR)/,$(addsuffix .pb.o,$(PB_NAMES)))

all: $(TARGET)

$(SRCDIR) $(INCDIR) $(OBJDIR) $(LIBDIR):
	mkdir -p $@

$(PB_CC) $(PB_H): $(PROTO_FILES) | $(SRCDIR) $(INCDIR)
	$(PROTOC) $(PROTO_PATH) --cpp_out=. $(PROTO_FILES)
	for f in *.pb.cc ; do mv -f $$f $(SRCDIR)/ ; done
	for f in *.pb.h  ; do mv -f $$f $(INCDIR)/ ; done

$(OBJDIR)/%.pb.o: $(SRCDIR)/%.pb.cc | $(OBJDIR)
	$(COMPILER) $(CXXFLAGS) -I$(INCDIR) $(DEPEND_INCLUDE) -c $< -o $@

$(TARGET): $(PB_CC) $(PB_H) $(OBJS) | $(LIBDIR)
	$(COMPILER) -shared $(OBJS) -o $@ $(PROTOBUF_LIBS)

# install
install: $(TARGET) $(DEPEND_PROTO)
	install -d $(INSTALL_INCLUDEDIR)
	install -d $(INSTALL_LIBDIR)

	install -m 644 $(PB_H) $(INSTALL_INCLUDEDIR)/
	install -m 755 $(TARGET) $(INSTALL_LIBDIR)/

# clean
clean:
	rm -rf $(INCDIR) $(SRCDIR) $(OBJDIR) $(LIBDIR)

# uninstall
UNINSTALL_HEADERS := $(notdir $(PB_H))
uninstall:
	@for f in $(UNINSTALL_HEADERS); do \
		rm -f $(INSTALL_INCLUDEDIR)/$$f; \
	done
	rm -f $(INSTALL_LIBDIR)/$(notdir $(TARGET))

.PHONY: all install uninstall clean
