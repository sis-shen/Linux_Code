.PHONY:all
all:sever client

sever:sever.cpp
	g++ -o $@ $^ -std=c++11
client:client.cpp
	g++ -o $@ $^ -std=c++11

.PHONY:cl 
cl:
	rm -f sever client