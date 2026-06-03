UBUNTU_VERSION := $(shell . /etc/os-release && echo $$VERSION_ID | cut -d. -f1)

CPLUS_VER=c++17
ifeq ($(shell [ $(UBUNTU_VERSION) -le 22 ] && echo yes),yes)
  ROS_DISTRO := $(shell echo $$ROS_DISTRO)
  ifneq ($(ROS_DISTRO),humble)
    CPLUS_VER=c++14
  endif
endif

CFLAGS  =  -std=$(CPLUS_VER) -shared -fPIC

LIBS           =  -I../../../tupleLib/include

all:
	g++ $(CFLAGS) -fPIC -o UF_TO_NUMBER.so UF_TO_NUMBER.cpp
	g++ $(CFLAGS) -fPIC -o UF_TO_LONG_NUMBER.so UF_TO_LONG_NUMBER.cpp
	g++ $(CFLAGS) -fPIC -o UF_TO_DOUBLE_NUMBER.so UF_TO_DOUBLE_NUMBER.cpp
	g++ $(CFLAGS) -fPIC -o UF_TO_STRING.so UF_TO_STRING.cpp
	g++ $(CFLAGS) -fPIC -o UF_TO_CHAR.so UF_TO_CHAR.cpp $(LIBS)
	g++ $(CFLAGS) -fPIC -o UF_TO_NUMBER.so UF_TO_NUMBER.cpp $(LIBS)

clean:
	rm -f *.o *.so

