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
	g++  $(CFLAGS) UF_ARRAY_AGG_INT.cpp $(LIBS) -o UF_ARRAY_AGG_INT.so
	g++  $(CFLAGS) UF_ARRAY_AGG_LONG.cpp $(LIBS) -o UF_ARRAY_AGG_LONG.so
	g++  $(CFLAGS) UF_ARRAY_AGG_DOUBLE.cpp $(LIBS) -o UF_ARRAY_AGG_DOUBLE.so
	g++  $(CFLAGS) UF_ARRAY_AGG_STRING.cpp $(LIBS) -o UF_ARRAY_AGG_STRING.so
	g++  $(CFLAGS) UF_AVG.cpp $(LIBS) -o UF_AVG.so
	g++  $(CFLAGS) UF_MIN.cpp $(LIBS) -o UF_MIN.so
	g++  $(CFLAGS) UF_MAX.cpp $(LIBS) -o UF_MAX.so
	g++  $(CFLAGS) UF_COUNT.cpp $(LIBS) -o UF_COUNT.so
	g++  $(CFLAGS) UF_SUM.cpp $(LIBS) -o UF_SUM.so
	g++  $(CFLAGS) UF_STRING_AGG.cpp $(LIBS) -o UF_STRING_AGG.so

clean:
	rm -f *.o *.so

