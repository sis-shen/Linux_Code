mycmd: main.o logger.o
	g++ -o $@ *.o -std=c++17 -lpthread -letcd-cpp-api -lspdlog -lssl -lcrypto -lgrpc++ -lprotobuf -lfmt -lcpprest

main.o:main.cpp
	g++ -c main.cpp -o main.o

logger.o:logger.cpp
	g++ -c logger.cpp -o logger.o

.PHONY:clean
clean:
	rm -f mycmd *.o