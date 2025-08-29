ASM=nasm


boot.bin: ./boot/boot.asm
	$(ASM) -f bin ./boot/boot.asm -o boot.bin


cl:
	rm -r ./build/*