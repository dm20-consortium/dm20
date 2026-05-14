

ifeq ($(shell cat /etc/os-release | grep VERSION_ID | sed -e "s/^[A-Z]*\_[A-Z]*\=\"\([0-9]*\)\.[0-9]*\"/\1/"),24)
  CFLAGS  =  -std=c++17 -shared -fPIC
else
  CFLAGS  =  -std=c++14 -shared -fPIC
endif

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

