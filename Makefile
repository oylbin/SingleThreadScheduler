OS ?= $(shell uname -s)

ifeq ($(OS),Windows_NT)
	CONAN_PRESET:=conan-default
else
	CONAN_PRESET:=conan-debug
endif

.PHONY: build clean test

build:
	mkdir -p build 
	conan install . --build=missing -s build_type=Debug -o singlethreadscheduler/*:test=True
	cmake --preset $(CONAN_PRESET) -DBUILD_TESTING=ON
	cmake --build --preset conan-debug

clean:
	rm -rf build

test:
	./build/Debug/singlethreadschedulertest


conan:
	conan install . --build=missing
	conan create .
	conan list "singlethreadscheduler/*:*"
