

ifeq ($(shell cat /etc/os-release | grep VERSION_ID | sed -e "s/^[A-Z]*\_[A-Z]*\=\"\([0-9]*\)\.[0-9]*\"/\1/"),24)
  CFLAGS  =  -std=c++17 -shared -fPIC
  GEOS_VER = 312
else
  CFLAGS  =  -std=c++14 -shared -fPIC
  ifeq ($(shell cat /etc/os-release | grep VERSION_ID | sed -e "s/^[A-Z]*\_[A-Z]*\=\"\([0-9]*\)\.[0-9]*\"/\1/"),18)
    GEOS_VER = 36
  else
    GEOS_VER = 38
  endif
endif

LIBS           =  -I../../../tupleLib/include -I../../../dmClientLib/include

all:
	g++ $(CFLAGS) -c CoordStrFormat.cpp  $(LIBS)
	g++ $(CFLAGS) -o UF_MakePoint.so UF_MakePoint.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_MakeLine.so UF_MakeLine.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_MakeLine_Union.so UF_MakeLine_Union.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_MakePolygon.so UF_MakePolygon.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_Boundary.so UF_Boundary.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_StartPoint.so UF_StartPoint.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_EndPoint.so UF_EndPoint.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_ClosestPoint.so UF_ClosestPoint.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -c GeoAccessor.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_X.so UF_X.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_Y.so UF_Y.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_Z.so UF_Z.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_XMax.so UF_XMax.cpp GeoAccessor.o $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_YMax.so UF_YMax.cpp GeoAccessor.o $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_ZMax.so UF_ZMax.cpp GeoAccessor.o $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_XMin.so UF_XMin.cpp GeoAccessor.o $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_YMin.so UF_YMin.cpp GeoAccessor.o $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_ZMin.so UF_ZMin.cpp GeoAccessor.o $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_Distance.so UF_Distance.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_Distance_For_CooL4.so UF_Distance_For_CooL4.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_Distance_4326.so UF_Distance_4326.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_Distance_signal_4326.so UF_Distance_signal_4326.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_XY_DISTANCE.so UF_XY_DISTANCE.cpp 
	g++ $(CFLAGS) -o UF_UOT_TO_STATION.so UF_UOT_TO_STATION.cpp
	g++ $(CFLAGS) -o UF_STATION_TO_UOT.so UF_STATION_TO_UOT.cpp $(LIBS)
	g++ $(CFLAGS) -o UF_Azimuth.so -DGEOS_VER=${GEOS_VER} UF_Azimuth.cpp $(LIBS)
	g++ $(CFLAGS) -o UF_Centroid.so UF_Centroid.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_Contains.so UF_Contains.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_Line_Length.so UF_Line_Length.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_Intersects.so UF_Intersects.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_Buffer.so UF_Buffer.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_Union.so UF_Union.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_Union2.so UF_Union2.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_Union_in_route.so UF_Union_in_route.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_Line_Interpolate_Point.so UF_Line_Interpolate_Point.cpp $(LIBS) -lgeos_c
	g++ $(CFLAGS) -o UF_Line_Locate_Point.so UF_Line_Locate_Point.cpp $(LIBS) -lpqxx -lpq
	g++ $(CFLAGS) -o UF_GCS_TO_JGD2011.so UF_GCS_TO_JGD2011.cpp $(LIBS) -lpqxx -lpq
	g++ $(CFLAGS) -o UF_LON_LAT_TO_XY.so UF_LON_LAT_TO_XY.cpp $(LIBS) -lpqxx -lpq
	g++ $(CFLAGS) -o UF_GET_FREESPACE_BY_LANE_REL.so UF_GET_FREESPACE_BY_LANE_REL.cpp $(LIBS) -lpqxx -lpq
	g++ $(CFLAGS) -o UF_JGD2011_TO_GCS.so UF_JGD2011_TO_GCS.cpp $(LIBS) -lpqxx -lpq
	g++ $(CFLAGS) -o UF_GeomFromText.so UF_GeomFromText.cpp $(LIBS) 
	g++ $(CFLAGS) -o UF_SetSRID.so UF_SetSRID.cpp $(LIBS) 

clean:
	rm -f *.o *.so

