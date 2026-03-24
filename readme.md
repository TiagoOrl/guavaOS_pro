## install:
sudo apt install qemu-system-x86 && 
sudo apt install nasm


## cross compiler setup:
sudo apt install build-essential &&
sudo apt install bison &&
sudo apt install flex &&
sudo apt install libgmp3-dev &&
sudo apt install libmpc-dev &&
sudo apt install libmpfr-dev &&
sudo apt install texinfo &&
sudo apt install libisl-dev


download binutils-2.35.1
download gcc-10.2.0

## run in terminal:

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"


cd $HOME/src

mkdir build-binutils
cd build-binutils
../binutils-x.y.z/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make
make install