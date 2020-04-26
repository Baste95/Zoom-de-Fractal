app: projet_MPL.cpp
	g++ -Wall -std=c++14 -O3 -pthread projet_MPL.cpp -o app `pkg-config --cflags --libs opencv`


