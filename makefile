ASM=nasm


boot.bin: ./boot/boot.asm
	$(ASM) -f bin ./boot/boot.asm -o boot.bin
	dd if=./msg.txt >> ./boot.bin
#	write bytes after the message to fill the 512 sector bytes requirement
	dd if=/dev/zero bs=512 count=1 >> ./boot.bin 


cl:
	rm -r ./build/* boot.bin