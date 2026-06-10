ASM=nasm
FILES= ./build/kernel.asm.o ./build/kernel.o ./build/gdt/gdt.o ./build/task/task.asm.o ./build/task/tss.asm.o ./build/gdt/gdt.asm.o ./build/memory/paging/paging.asm.o ./build/idt/idt.asm.o ./build/idt/idt.o ./build/disk/disk.o ./build/memory/paging/paging.o ./build/memory/memory.o ./build/memory/heap/kheap.o ./build/memory/heap/heap.o ./build/io/io.asm.o ./build/fs/pparser.o ./build/string/string.o ./build/disk/streamer.o ./build/fs/file.o ./build/fs/fat/fat16.o ./build/task/task.o ./build/task/process.o
INCLUDES = -I./src
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc


all: ./bin/boot.bin ./bin/kernel.bin
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
#	fill 16 sectors with 1048576 bytes of zeroes
	dd if=/dev/zero count=16 bs=1048576 >> ./bin/os.bin 
	sudo mount -t vfat ./bin/os.bin /mnt/d
# copy a file to the OS bin
	sudo cp ./hello.txt /mnt/d
	sudo umount /mnt/d

./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc $(FLAGS) -T ./src/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o

./bin/boot.bin: ./src/boot/boot.asm
	$(ASM) -f bin ./src/boot/boot.asm -o ./bin/boot.bin -g

./build/kernel.asm.o: ./src/kernel.asm
	$(ASM) -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o


./build/idt/idt.asm.o: ./src/idt/idt.asm
	$(ASM) -f elf -g ./src/idt/idt.asm -o ./build/idt/idt.asm.o

./build/io/io.asm.o: ./src/io/io.asm
	$(ASM) -f elf -g ./src/io/io.asm -o ./build/io/io.asm.o

./build/task/tss.asm.o: ./src/task/tss.asm
	$(ASM) -f elf -g ./src/task/tss.asm -o ./build/task/tss.asm.o

./build/task/task.asm.o: ./src/task/task.asm
	$(ASM) -f elf -g ./src/task/task.asm -o ./build/task/task.asm.o

./build/gdt/gdt.asm.o: ./src/gdt/gdt.asm
	$(ASM) -f elf -g ./src/gdt/gdt.asm -o ./build/gdt/gdt.asm.o

./build/memory/paging/paging.asm.o: ./src/memory/paging/paging.asm
	$(ASM) -f elf -g ./src/memory/paging/paging.asm -o ./build/memory/paging/paging.asm.o

./build/kernel.o: ./src/kernel.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel.c -o ./build/kernel.o -g

./build/idt/idt.o: ./src/idt/idt.c
	i686-elf-gcc $(INCLUDES) -I./src/idt $(FLAGS) -std=gnu99 -c ./src/idt/idt.c -o ./build/idt/idt.o -g

./build/memory/memory.o: ./src/memory/memory.c
	i686-elf-gcc $(INCLUDES) -I./src/memory $(FLAGS) -std=gnu99 -c ./src/memory/memory.c -o ./build/memory/memory.o -g

./build/task/task.o: ./src/task/task.c
	i686-elf-gcc $(INCLUDES) -I./src/task $(FLAGS) -std=gnu99 -c ./src/task/task.c -o ./build/task/task.o -g

./build/task/process.o: ./src/task/process.c
	i686-elf-gcc $(INCLUDES) -I./src/task $(FLAGS) -std=gnu99 -c ./src/task/process.c -o ./build/task/process.o -g

./build/memory/heap/heap.o: ./src/memory/heap/heap.c
	i686-elf-gcc $(INCLUDES) -I./src/memory/heap $(FLAGS) -std=gnu99 -c ./src/memory/heap/heap.c -o ./build/memory/heap/heap.o -g

./build/memory/heap/kheap.o: ./src/memory/heap/kheap.c
	i686-elf-gcc $(INCLUDES) -I./src/memory/heap $(FLAGS) -std=gnu99 -c ./src/memory/heap/kheap.c -o ./build/memory/heap/kheap.o -g

./build/memory/paging/paging.o: ./src/memory/paging/paging.c
	i686-elf-gcc $(INCLUDES) -I./src/memory/paging $(FLAGS) -std=gnu99 -c ./src/memory/paging/paging.c -o ./build/memory/paging/paging.o -g

./build/disk/disk.o: ./src/disk/disk.c
	i686-elf-gcc $(INCLUDES) -I./src/disk $(FLAGS) -std=gnu99 -c ./src/disk/disk.c -o ./build/disk/disk.o -g

./build/disk/streamer.o: ./src/disk/streamer.c
	i686-elf-gcc $(INCLUDES) -I./src/disk $(FLAGS) -std=gnu99 -c ./src/disk/streamer.c -o ./build/disk/streamer.o -g

./build/string/string.o: ./src/string/string.c
	i686-elf-gcc $(INCLUDES) -I./src/string $(FLAGS) -std=gnu99 -c ./src/string/string.c -o ./build/string/string.o -g

./build/fs/pparser.o: ./src/fs/pparser.c
	i686-elf-gcc $(INCLUDES) -I./fs $(FLAGS) -std=gnu99 -c ./src/fs/pparser.c -o ./build/fs/pparser.o -g

./build/fs/file.o: ./src/fs/file.c
	i686-elf-gcc $(INCLUDES) -I./fs $(FLAGS) -std=gnu99 -c ./src/fs/file.c -o ./build/fs/file.o -g

./build/fs/fat/fat16.o: ./src/fs/fat/fat16.c
	i686-elf-gcc $(INCLUDES) -I./fs/fat $(FLAGS) -std=gnu99 -c ./src/fs/fat/fat16.c -o ./build/fs/fat/fat16.o -g

./build/gdt/gdt.o: ./src/gdt/gdt.c
	i686-elf-gcc $(INCLUDES) -I./gdt $(FLAGS) -std=gnu99 -c ./src/gdt/gdt.c -o ./build/gdt/gdt.o -g



cl:
	find ./build/ -type f -delete
	rm -rf ./bin/*.bin
