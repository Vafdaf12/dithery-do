
all:
	make configure
	make build

build:
	cmake --build build/

configure:
	cmake -B build -S .

.PHONY: build run all

