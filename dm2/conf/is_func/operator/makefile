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
	g++ $(CFLAGS) UF_DIVIDE.cpp $(LIBS) -o UF_DIVIDE.so
	g++ $(CFLAGS) UF_FACTORIAL.cpp $(LIBS) -o UF_FACTORIAL.so
	g++ $(CFLAGS) UF_IN.cpp -o UF_IN.so
	g++ $(CFLAGS) UF_LIKE.cpp $(LIBS) -o UF_LIKE.so
	g++ $(CFLAGS) UF_MINUS.cpp $(LIBS) -o UF_MINUS.so
	g++ $(CFLAGS) UF_PLUS.cpp $(LIBS) -o UF_PLUS.so
	g++ $(CFLAGS) UF_PRODUCT.cpp $(LIBS) -o UF_PRODUCT.so
	g++ $(CFLAGS) UF_SIGN_INVERSION.cpp $(LIBS) -o UF_SIGN_INVERSION.so
	g++ $(CFLAGS) UF_ROW_NUMBER_ASC.cpp $(LIBS) -o UF_ROW_NUMBER_ASC.so
	g++ $(CFLAGS) UF_ROW_NUMBER_DESC.cpp $(LIBS) -o UF_ROW_NUMBER_DESC.so


clean:
	rm -f *.o *.so

