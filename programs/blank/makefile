


all:
	nasm -f elf ./blank.asm -o ./build/blank.o
	i686-elf-gcc -g -T ./linker.ld -o ./blank.bin --freestanding -O0 -nostdlib -fpic -g ./build/blank.o

cl:
	rm -rf ./build/blank.o ./blank.bin