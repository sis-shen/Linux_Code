# cmd:main.o header.o
# 	g++ -o $@ *.o -lgflags

# main.o:main.cpp
# 	g++ -c main.cpp -o main.o

# header.o:header.cpp
# 	g++ -c header.cpp -o header.o

# .PHONY:clean
# clean:
# 	rm -f cmd *.o


cmd:main.cpp
	g++ -o $@ $^ -lgflags

.PHONY:clean
clean:
	rm -f cmd