myos.iso: boot.o kernel.o myos
	mkdir -p isodir/boot/grub
	cp myos isodir/boot/myos
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso isodir

boot.o:
	nasm -f elf32 boot.asm -o boot.o

kernel.o:
	i686-elf-gcc -c src/stdlib.c -o stdlib.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -c src/vga.c -o vga.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

myos:
	i686-elf-gcc -T linker.ld -o myos -ffreestanding -O2 -nostdlib boot.o kernel.o vga.o stdlib.o -lgcc

clean:
	rm myos myos.iso boot.o kernel.o vga.o stdlib.o
