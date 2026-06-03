
#/etc/os-release���烁�W���[�o�[�W�����𔻒肷��
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
	g++ $(CFLAGS) UF_CONCAT.cpp $(LIBS) -o UF_CONCAT.so
	g++ $(CFLAGS) UF_SUBSTR.cpp $(LIBS) -o UF_SUBSTR.so
	g++ $(CFLAGS) UF_LEFT.cpp $(LIBS) -o UF_LEFT.so
	g++ $(CFLAGS) UF_RIGHT.cpp $(LIBS) -o UF_RIGHT.so
	g++ $(CFLAGS) UF_LENGTH.cpp $(LIBS) -o UF_LENGTH.so
	g++ $(CFLAGS) UF_LOWER.cpp $(LIBS) -o UF_LOWER.so
	g++ $(CFLAGS) UF_UPPER.cpp $(LIBS) -o UF_UPPER.so

clean:
	rm -f *.o *.so

