CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -g -O0
LFLAGS = -ffreestanding -O2 -nostdlib
ZIGFLAGS = -target x86-freestanding-none -mcpu=i386 -O ReleaseSmall
ZIGFLAGS_DEBUG = -target x86-freestanding-none -mcpu=i386 -O Debug

LD = i686-elf-gcc -T linker.ld -o

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
	i686-elf-gcc -c $(LIBKERNEL)/memmove.c -o memmove.o $(CFLAGS)
	i686-elf-gcc -c $(LIBKERNEL)/memcpy.c -o memcpy.o $(CFLAGS)
	i686-elf-gcc -c $(LIBKERNEL)/memcmp.c -o memcmp.o $(CFLAGS)
	i686-elf-gcc -c $(LIBKERNEL)/printf.c -o printf.o $(CFLAGS)
	i686-elf-gcc -c $(LIBKERNEL)/kmalloc.c -o kmalloc.o $(CFLAGS)
	i686-elf-gcc -c $(LIBKERNEL)/kpanic.c -o kpanic.o $(CFLAGS)
	i686-elf-gcc -c $(LIBKERNEL)/iob.c -o iob.o $(CFLAGS)
	i686-elf-gcc -c $(LIBKERNEL)/math.c -o math.o $(CFLAGS)
	#i686-elf-gcc -c $(LIBKERNEL)/kfree.c -o kfree.o $(CFLAGS)
	zig build-obj $(LIBKERNEL)/kfree.zig $(ZIGFLAGS_DEBUG) -femit-bin=kfree.o
	zig build-obj $(CKERNEL)/fs/fs.zig $(ZIGFLAGS_DEBUG) -femit-bin=fs.o
	zig build-obj $(CKERNEL)/fs/ext2.zig $(ZIGFLAGS_DEBUG) -femit-bin=ext2.o
	zig build-obj $(CKERNEL)/fs/fat12.zig $(ZIGFLAGS_DEBUG) -femit-bin=fat12.o
	zig build-obj $(CKERNEL)/fs/fs_init.zig $(ZIGFLAGS_DEBUG) -femit-bin=fs_init.o
	zig build-obj $(CKERNEL)/timer/timer.zig $(ZIGFLAGS_DEBUG) -femit-bin=timer.o
	zig build-obj $(CKERNEL)/drivers/floppy.zig $(ZIGFLAGS_DEBUG) -femit-bin=floppy.o
	zig build-obj $(CKERNEL)/drivers/ata.zig $(ZIGFLAGS_DEBUG) -femit-bin=ata.o
	i686-elf-gcc -c $(CKERNEL)/vga/vga.c -o vga.o $(CFLAGS)
	i686-elf-gcc -c $(CKERNEL)/vga/text_cursor.c -o text_cursor.o $(CFLAGS)
	i686-elf-gcc -c $(CKERNEL)/dt/gdt.c -o gdt.o $(CFLAGS)
	i686-elf-gcc -c $(CKERNEL)/dt/idt.c -o idt.o $(CFLAGS)
	i686-elf-gcc -c $(CKERNEL)/memory/vmm.c -o vmm.o $(CFLAGS)
	i686-elf-gcc -c $(CKERNEL)/memory/pmm.c -o pmm.o $(CFLAGS)
	i686-elf-gcc -c $(CKERNEL)/drivers/keyboard.c -o keyboard.o $(CFLAGS)
	i686-elf-gcc -c $(CKERNEL)/dt/pic/send_eoi.c -o send_eoi.o $(CFLAGS)
	i686-elf-gcc -c kernel/kernel.c -o kernel.o $(CFLAGS)
	nasm -f elf32 -g kernel/src/dt/gdt.asm -o gdtasm.o
	nasm -f elf32 -g kernel/src/dt/idt.asm -o idtasm.o
	nasm -f elf32 -g kernel/src/memory/paging.asm -o paging.o
	nasm -f elf32 -g kernel/src/user/usermode.asm -o usermode.o

myos:
	$(LD) myos $(LFLAGS) *.o -lgcc

clean:
	rm myos myos.iso *.o

#
# non-compile section
init:
	export PREFIX="$HOME/opt/cross"
	export TARGET=i686-elf
	export PATH="$PREFIX/bin:$PATH"
	export PATH=$PATH:/snap/bin
start:
	qemu-system-i386 -fda floppy.img -drive file=disk.img,format=raw,if=ide -m 2048M -cdrom myos.iso -boot d
startdbg:
	qemu-system-i386 -fda floppy.img -drive file=disk.img,format=raw,if=ide -m 2048M -cdrom myos.iso -boot d -s -S

