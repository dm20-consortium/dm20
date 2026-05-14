

ifeq ($(shell cat /etc/os-release | grep VERSION_ID | sed -e "s/^[A-Z]*\_[A-Z]*\=\"\([0-9]*\)\.[0-9]*\"/\1/"),24)
  CFLAGS  =  -std=c++17 -shared -fPIC
else
  CFLAGS  =  -std=c++14 -shared -fPIC
endif

LIBS           =  -I../../../tupleLib/include

BITSTRING="BitString.o"

all:
	g++ $(CFLAGS) -c BitString.cpp $(LIBS) -Wall
	g++ $(CFLAGS) UF_MATCH_HIGH_BIT.cpp ${BITSTRING} $(LIBS) -o UF_MATCH_HIGH_BIT.so -Wall
	g++ $(CFLAGS) UF_MATCH_MID_BIT.cpp  ${BITSTRING} $(LIBS) -o UF_MATCH_MID_BIT.so -Wall
	g++ $(CFLAGS) UF_MATCH_LOW_BIT.cpp  ${BITSTRING} $(LIBS) -o UF_MATCH_LOW_BIT.so -Wall
	g++ $(CFLAGS) UF_MATCH_ALL_BIT.cpp  ${BITSTRING} $(LIBS) -o UF_MATCH_ALL_BIT.so -Wall
	g++ $(CFLAGS) UF_ABS.cpp $(LIBS)  -o UF_ABS.so
	g++ $(CFLAGS) UF_CBRT.cpp $(LIBS)  -o UF_CBRT.so
	g++ $(CFLAGS) UF_CEIL.cpp $(LIBS)  -o UF_CEIL.so
	g++ $(CFLAGS) UF_DEGREES.cpp $(LIBS)  -o UF_DEGREES.so
	g++ $(CFLAGS) UF_EXP.cpp $(LIBS)  -o UF_EXP.so
	g++ $(CFLAGS) UF_FLOOR.cpp $(LIBS)  -o UF_FLOOR.so
	g++ $(CFLAGS) UF_LN.cpp $(LIBS)  -o UF_LN.so
	g++ $(CFLAGS) UF_LOG.cpp $(LIBS)  -o UF_LOG.so
	g++ $(CFLAGS) UF_MOD.cpp $(LIBS)  -o UF_MOD.so
	g++ $(CFLAGS) UF_PI.cpp $(LIBS)  -o UF_PI.so
	g++ $(CFLAGS) UF_POWER.cpp $(LIBS)  -o UF_POWER.so
	g++ $(CFLAGS) UF_RADIANS.cpp $(LIBS)  -o UF_RADIANS.so
	g++ $(CFLAGS) UF_RANDOM.cpp $(LIBS)  -o UF_RANDOM.so
	g++ $(CFLAGS) UF_ROUND.cpp $(LIBS)  -o UF_ROUND.so
	g++ $(CFLAGS) UF_SIGN.cpp $(LIBS)  -o UF_SIGN.so
	g++ $(CFLAGS) UF_SQRT.cpp $(LIBS)  -o UF_SQRT.so
	g++ $(CFLAGS) UF_TRUNC.cpp $(LIBS)  -o UF_TRUNC.so
	g++ $(CFLAGS) UF_ACOS.cpp $(LIBS)  -o UF_ACOS.so
	g++ $(CFLAGS) UF_ASIN.cpp $(LIBS)  -o UF_ASIN.so
	g++ $(CFLAGS) UF_ATAN.cpp $(LIBS)  -o UF_ATAN.so
	g++ $(CFLAGS) UF_ATAN2.cpp $(LIBS)  -o UF_ATAN2.so
	g++ $(CFLAGS) UF_COS.cpp $(LIBS)  -o UF_COS.so
	g++ $(CFLAGS) UF_COT.cpp $(LIBS)  -o UF_COT.so
	g++ $(CFLAGS) UF_SIN.cpp $(LIBS)  -o UF_SIN.so
	g++ $(CFLAGS) UF_TAN.cpp $(LIBS)  -o UF_TAN.so

clean:
	rm -f *.o *.so

