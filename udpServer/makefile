.PHONY:all
all:udpServer udpclient

udpServer:Main.cpp
	g++ -o $@ $^ -std=c++11
udpclient:UdpClient.cpp
	g++ -o $@ $^ -std=c++11 -lpthread
	

.PHONY:cl
cl:
	rm -f udpServer udpclient