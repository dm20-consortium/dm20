

ifeq ($(shell cat /etc/os-release | grep VERSION_ID | sed -e "s/^[A-Z]*\_[A-Z]*\=\"\([0-9]*\)\.[0-9]*\"/\1/"),24)
  CFLAGS  =  -std=c++17 -shared -fPIC
else
  CFLAGS  =  -std=c++14 -shared -fPIC
endif

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

