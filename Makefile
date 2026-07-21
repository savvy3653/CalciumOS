CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -g -O0
LFLAGS = -ffreestanding -O2 -nostdlib

CKERNEL =  kernel/src
LIBKERNEL = kernel/src/libc

myos.iso: boot.o kernel.o myos
	mkdir -p isodir/boot/grub
	cp myos isodir/boot/myos
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso isodir

boot.o:
	nasm -f elf32 kernel/boot.asm -o boot.o

kernel.o:
	i686-elf-gcc -c $(LIBKERNEL)/strlen.c -o strlen.o $(CFLAGS)
	i686-elf-gcc -c $(LIBKERNEL)/memset.c -o memset.o $(CFLAGS)
	i686-elf-gcc -c $(CKERNEL)/vga/vga.c -o vga.o $(CFLAGS)
	i686-elf-gcc -c $(CKERNEL)/dt/gdt.c -o gdt.o $(CFLAGS)
	i686-elf-gcc -c kernel/kernel.c -o kernel.o $(CFLAGS)
	nasm -f elf32 -g kernel/src/dt/gdt.asm -o gdtasm.o

myos:
	i686-elf-gcc -T linker.ld -o myos $(LFLAGS) boot.o kernel.o vga.o strlen.o memset.o gdt.o gdtasm.o -lgcc

clean:
	rm myos myos.iso *.o

#
# non-compile section
init:
	export PREFIX="$HOME/opt/cross"
	export TARGET=i686-elf
	export PATH="$PREFIX/bin:$PATH"
start:
	qemu-system-i386 -cdrom myos.iso
startdbg:
	qemu-system-i386 -cdrom myos.iso -s -S
