.PHONY:all
all:servercal clientcal

Lib = -ljsoncpp

servercal:ServerCal.cpp
	g++ -o $@ $^ -std=c++11 $(Lib)
clientcal:ClientCal.cpp
	g++ -o $@ $^ -std=c++11 $(Lib)
	

.PHONY:cl
cl:
	rm -f servercal clientcal