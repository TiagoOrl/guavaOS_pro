ASM=nasm
FILES= ./build/kernel.asm.o

all: ./bin/boot.bin $(FILES)
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
 
./bin/boot.bin: ./src/boot/boot.asm
	$(ASM) -f bin ./src/boot/boot.asm -o ./bin/boot.bin

./build/kernel.asm.o: ./src/kernel.asm
	$(ASM) -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o

cl:
	rm -r ./build/* *.bin
