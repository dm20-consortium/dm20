

ifeq ($(shell cat /etc/os-release | grep VERSION_ID | sed -e "s/^[A-Z]*\_[A-Z]*\=\"\([0-9]*\)\.[0-9]*\"/\1/"),24)
  CFLAGS  =  -std=c++17 -shared -fPIC
else
  CFLAGS  =  -std=c++14 -shared -fPIC
endif

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

