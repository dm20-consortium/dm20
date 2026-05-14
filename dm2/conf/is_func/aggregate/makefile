

ifeq ($(shell cat /etc/os-release | grep VERSION_ID | sed -e "s/^[A-Z]*\_[A-Z]*\=\"\([0-9]*\)\.[0-9]*\"/\1/"),24)
  CFLAGS  =  -std=c++17 -shared -fPIC
else
  CFLAGS  =  -std=c++14 -shared -fPIC
endif

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

