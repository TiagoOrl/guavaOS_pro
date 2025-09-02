ASM=nasm


loader.bin: ./boot/boot.asm
	$(ASM) -f bin ./boot/boot.asm -o loader.bin
	dd if=./msg.txt >> ./loader.bin
#	write bytes after the message to fill the 512 sector bytes requirement
	dd if=/dev/zero bs=512 count=1 >> ./loader.bin


cl:
	rm -r ./build/* *.bin
