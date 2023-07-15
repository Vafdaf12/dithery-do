TEST_FILE=examples/rainbow_cloud.jpg
OUT_FILE=examples/output3_m.jpg
PALETTE=TokyoNight.txt

all:
	make configure
	make build

build:
	cmake --build build/

configure:
	cmake -B build -S .

run:
	./build/DitheryDo -f ${TEST_FILE} -o ${OUT_FILE} -p ${PALETTE} --space ${space} --select ${select}
debug:
	gdb --args ./build/DitheryDo -f ${TEST_FILE} -o ${OUT_FILE} -p ${PALETTE} --color-space ${space} --select ${select}
shaders:
	./res/glslangValidator res/compute.glsl -S comp -V -o res/comp.spv

.PHONY: build run all configure

