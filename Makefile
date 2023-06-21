TEST_FILE=examples/rainbow_cloud.jpg
OUT_FILE=examples/output.jpg
PALETTE=palette.txt

all:
	make configure
	make build

build:
	cmake --build build/

configure:
	cmake -B build -S .

run:
	./build/DitheryDo -f ${TEST_FILE} -o ${OUT_FILE} -p ${PALETTE} --color-space ${space} --select ${select}

.PHONY: build run all configure

