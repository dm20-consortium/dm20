

ifeq ($(shell cat /etc/os-release | grep VERSION_ID | sed -e "s/^[A-Z]*\_[A-Z]*\=\"\([0-9]*\)\.[0-9]*\"/\1/"),24)
  CFLAGS  =  -std=c++17 -shared -fPIC
else
  CFLAGS  =  -std=c++14 -shared -fPIC
endif

LIBS           =  -I../../../tupleLib/include

all:
	g++ $(CFLAGS) -c GridMap.cpp $(LIBS)
	g++ $(CFLAGS) -c UF_DATA_FUSION.cpp $(LIBS)
	g++ $(CFLAGS) -o UF_DATA_FUSION.so UF_DATA_FUSION.o GridMap.o $(LIBS)
	g++ $(CFLAGS) -o UF_DATA_FUSION2.so UF_DATA_FUSION2.cpp $(LIBS) 
	g++ $(CFLAGS) -o UF_DATA_FUSION3.so UF_DATA_FUSION3.cpp $(LIBS) 
	
clean:
	rm -f *.o *.so

