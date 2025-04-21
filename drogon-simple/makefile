.PHONY: clean rebuild

rebuild: clean build
	@cd build && cmake .. && make -j$(nproc)

clean:
	rm -rf build

build:
	mkdir -p build