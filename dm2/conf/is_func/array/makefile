
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
	g++ $(CFLAGS) UF_ARRAY_APPEND.cpp $(LIBS) -o UF_ARRAY_APPEND.so
	g++ $(CFLAGS) UF_ARRAY_REMOVE.cpp $(LIBS) -o UF_ARRAY_REMOVE.so
	g++ $(CFLAGS) UF_ARRAY_REPLACE.cpp $(LIBS) -o UF_ARRAY_REPLACE.so
	g++ $(CFLAGS) UF_ARRAY_CAT.cpp $(LIBS) -o UF_ARRAY_CAT.so
	g++ $(CFLAGS) UF_ARRAY_LENGTH.cpp $(LIBS) -o UF_ARRAY_LENGTH.so
	g++ $(CFLAGS) UF_ARRAY_TO_STRING.cpp $(LIBS) -o UF_ARRAY_TO_STRING.so
	g++ $(CFLAGS) UF_STR_ARRAY_APPEND.cpp $(LIBS) -o UF_STR_ARRAY_APPEND.so
	g++ $(CFLAGS) UF_STRING_TO_ARRAY.cpp $(LIBS) -o UF_STRING_TO_ARRAY.so
	g++ $(CFLAGS) UF_REFVEC.cpp $(LIBS) -o UF_REFVEC.so
	g++ $(CFLAGS) UF_REFVEC_STRING.cpp $(LIBS) -o UF_REFVEC_STRING.so

clean:
	rm -f *.o *.so

