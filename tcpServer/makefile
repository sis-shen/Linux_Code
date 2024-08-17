.PHONY:all
all:tcpserverd tcpclient

tcpserverd:Main.cpp
	g++ -o $@ $^ -std=c++11 -lpthread
tcpclient:TcpClient.cpp
	g++ -o $@ $^ -std=c++11 -lpthread
	

.PHONY:cl
cl:
	rm -f tcpserverd tcpclient