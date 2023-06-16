
all:
	make configure
	make build

build:
	cmake -B build -S . 

configure:
	cmake -B build -S .

.PHONY: build run all

