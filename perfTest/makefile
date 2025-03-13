cmd:main.cpp
	g++ -o $@ $^ 

.PHONY:clean
clean:
	rm -f cmd perf.*

test:cmd
	sudo perf record -F 99 -g ./cmd --sleep 60

summon:perf.data
	sudo perf script -i perf.data &> perf.unfold && \
	sudo ./FlameGraph/stackcollapse-perf.pl perf.unfold &> perf.folded && \
	sudo ./FlameGraph/flamegraph.pl perf.folded > perf.svg