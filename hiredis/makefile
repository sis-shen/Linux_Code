hello:hello.cxx
	g++ -std=c++17 -o $@ $^ /usr/local/lib/libredis++.a /usr/lib/x86_64-linux-gnu/libhiredis.a  -pthread

.PHONY:clean
clean:
	rm -f hello
	