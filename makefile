ASM=nasm
FILES= ./build/kernel.asm.o

all: ./bin/boot.bin ./bin/kernel.bin
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
#	fill 100 sectors with 512 bytes of zeroes
	dd if=/dev/zero count=100 bs=512 >> ./bin/os.bin 

./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc -T ./src/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o

./bin/boot.bin: ./src/boot/boot.asm
	$(ASM) -f bin ./src/boot/boot.asm -o ./bin/boot.bin

./build/kernel.asm.o: ./src/kernel.asm
	$(ASM) -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o

cl:
	rm -r ./build/* *.bin
