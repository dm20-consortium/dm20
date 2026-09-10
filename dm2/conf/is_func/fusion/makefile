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
	g++ $(CFLAGS) -c GridMap.cpp $(LIBS)
	g++ $(CFLAGS) -c UF_DATA_FUSION.cpp $(LIBS)
	g++ $(CFLAGS) -o UF_DATA_FUSION.so UF_DATA_FUSION.o GridMap.o $(LIBS)
	g++ $(CFLAGS) -o UF_DATA_FUSION2.so UF_DATA_FUSION2.cpp $(LIBS) 
	g++ $(CFLAGS) -o UF_DATA_FUSION3.so UF_DATA_FUSION3.cpp $(LIBS) 
	
clean:
	rm -f *.o *.so

