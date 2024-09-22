.PHONY: build clean test

build:
	mkdir -p build 
	conan install . --build=missing -s build_type=Debug -o singlethreadscheduler/*:test=True
	cmake --preset conan-debug -DBUILD_TESTING=ON
	cmake --build --preset conan-debug

clean:
	rm -rf build

test:
	./build/Debug/singlethreadschedulertest


conan:
	conan install . --build=missing
	conan create .
	conan list "singlethreadscheduler/*:*"