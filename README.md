# SheetOS
This is a hobby OS based on *monolithic kernel* concept and *x86 architecture*.
SheetOS is written in NASM, C and Zig languages.
Now this system actually can do *nothing*. Although you can type text.
<img width="706" height="399" alt="image" src="https://github.com/user-attachments/assets/f06d1f3f-08bb-4f8a-b7d7-f0614690c289" />

## Features (what is implemented by far)
Technical:
- x86
- GRUB bootloader
- Protected Mode
- IDT/GDT setup
- VGA (*in progress*)
- Virtual memory manager
  
Drivers:
- Keyboard driver
- Floppy disk driver (1.44 MB floppies only)

Filesystem:
- ext2 (*in progress*)

## Getting Started
Dependencies:
- 'i686-elf-gcc' -- cross-compiler (https://wiki.osdev.org/GCC_Cross-Compiler)
- 'nasm'
- 'zig'
- 'make'
- 'xorriso'
- 'grub-mkrescue'
- 'qemu-system-i386'

### Ubuntu/Debian/(WSL)
```bash
sudo apt install nasm make xorriso grub-pc-bin qemu-system-x86
```
Cross-compiler `i686-elf-gcc` and `zig` are setup separately — links above + https://ziglang.org/download/.

### Versions
`GCC` (15.2.0)
`Zig` (0.16.0)

### How to actually start it
Use `make` command in terminal (in main OS directory) and then if it succeed use `make start`.

## Notes
GUI won't be provided. This is some kind of a UNIX-like system.
For debug you can use `make startdbg` command.
