.PHONY:all
all:servercal clientcal

servercal:ServerCal.cpp
	g++ -o $@ $^ -std=c++11 
clientcal:ClientCal.cpp
	g++ -o $@ $^ -std=c++11 
	

.PHONY:cl
cl:
	rm -f servercal clientcal