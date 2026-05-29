

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
make cl
make all

qemu-system-i386 -hda ./bin/os.bin


# target remote | qemu-system-x86_64 -hda ./loader.bin -S -gdb stdio