linuxpy:
	swig -c++ -python bigNumber.i
	g++ -fPIC -c bigNumber.cpp
	g++ -fPIC -c bigNumber_wrap.cxx -I/usr/include/python2.7
	g++ -shared bigNumber.o bigNumber_wrap.o -o _bigNumber.so
	
linuxdyn: main.cpp bigNumber.cpp
	g++ -c -fPIC bigNumber.cpp
	g++ -shared -o libbignumber.so bigNumber.o
	LD_LIBRARY_PATH=./ g++ main.cpp -lbignumber -I. -Wl,-rpath,. -L. -o TCHMK_1
	
windowspy:
	swig -c++ -python bigNumber.i
	g++ -c bigNumber.cpp
	g++ -c bigNumber_wrap.cxx -IC:\Python27\include
	g++ bigNumber.o bigNumber_wrap.o -Ic:\python27\include -Lc:\python27\libs -lpython27 -shared -o _bigNumber.pyd
	
windowsdyn: main.cpp bigNumber.cpp
	g++ -c bigNumber.cpp
	g++ -shared -o bignumber.dll bigNumber.o
	g++ main.cpp bignumber.dll -I. -Wl,-rpath,. -L. -o TCHMK_1.exe