.PHONY: build clean test

build:
	mkdir -p build
	cd build && cmake .. && cmake --build .

clean:
	rm -rf build

test:
	cd build && ctest