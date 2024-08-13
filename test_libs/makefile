lib = mymathlib.a

$(lib):add.o sub.o 
	gcc -shared -o libmymath.so *.o 

add.o:add.c 
	gcc -fPIC -c add.c -o add.o 

sub.o:sub.c
	gcc -fPIC -c sub.c -o sub.o

.PHONY:clean
clean:
	rm -f *.o *.a 

.PHONY:output 
output:
	mkdir -p lib/include
	mkdir -p lib/mymathlib
	cp *.h lib/include
	cp *.so lib/mymathlib