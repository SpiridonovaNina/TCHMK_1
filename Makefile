all: main.cpp bigNumber.cpp
	g++ -c -fPIC bigNumber.cpp
	g++ -shared -o libbignumber.so bigNumber.o
	LD_LIBRARY_PATH=./ g++ main.cpp -lbignumber -I. -Wl,-rpath,. -L. -o TCHMK_1
	
windows: main.cpp bigNumber.cpp
	g++ -c bigNumber.cpp
	g++ -shared -o bignumber.dll bigNumber.o
	g++ main.cpp bignumber.dll -I. -Wl,-rpath,. -L. -o TCHMK_1.exe