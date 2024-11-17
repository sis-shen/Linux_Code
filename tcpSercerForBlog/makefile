.PHONY:all
all:server client

server:Main.cpp
	g++ -o $@ $^ -std=c++11 
client:TCPClient.cpp
	g++ -o $@ $^ -std=c++11 
	

.PHONY:clean
clean:
	rm -f server client