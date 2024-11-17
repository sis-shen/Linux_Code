mycmd:main.o add.o sub.o
	g++ -o mycmd *.o

main.o:main.cpp
	g++ -c main.cpp -o main.o

add.o:add.cpp
	g++ -c add.cpp -o add.o

sub.o:sub.cpp
	g++ -c sub.cpp -o sub.o

.PHONY:clean
clean:
	rm -f mycmd *.o