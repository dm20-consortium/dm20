# common.mk
# tupleLib, dmClinetLib, is, apl の CPLUS_VERのバージョンを設定する
# 
# a) make CPLUS_VER=c++17のようにコマンドラインで指定されていたら、指定値に従う
# b) Ubuntu のバージョンが24以上: C++17
# c) Ubuntu のバージョンが22以下の場合
#     c-1 ) ROS2のhumbleがインストールされている場合は、c++17
#     c-2 ) ROS2のhumbleがインストールされていない場合は、c++14

# /etc/os-releaseからUbuntuメジャーバージョンを取得
UBUNTU_VERSION := $(shell . /etc/os-release && echo $${VERSION_ID%%.*})

ifeq ($(origin CPLUS_VER), undefined)
  CPLUS_VER := c++17

  ifeq ($(shell [ $(UBUNTU_VERSION) -le 22 ] && echo yes),yes)
    ROS_DISTRO := $(shell echo $$ROS_DISTRO)
    ifneq ($(ROS_DISTRO),humble)
      CPLUS_VER := c++14
    endif
  endif
endif


ifdef PROTO_PATH
PROTOBUF_CFLAGS  := -I$(PROTO_PATH)/protoc/include
PROTOBUF_LDFLAGS := -L$(PROTO_PATH)/protoc/lib
PROTOBUF_LIBS    := -Wl,--whole-archive $(PROTO_PATH)/protoc/lib/libprotobuf.a -Wl,--no-whole-archive
else
PROTOBUF_CFLAGS  := $(shell pkg-config --cflags protobuf)
PROTOBUF_LDFLAGS := $(shell pkg-config --libs-only-L protobuf)
PROTOBUF_LIBS    := $(shell pkg-config --libs protobuf)
endif
