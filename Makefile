all: main.cpp bigNumber.cpp
	g++ main.cpp bigNumber.cpp -o TCHMK_1 -lm
	
windows: main.cpp bigNumber.cpp
	g++ main.cpp bigNumber.cpp -o TCHMK_1.exe -lm
