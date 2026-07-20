extern kernel_main

ALIGN_FLAG  equ 1<<0                   ; align loaded modules on page boundaries
MEMINFO     equ 1<<1                   ; provide memory map
FLAGS       equ ALIGN_FLAG | MEMINFO   ; this is the Multiboot 'flag' field
MAGIC       equ 0x1BADB002             ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)       ; checksum of above, to prove we are multiboot


section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM


section .bss
align 16
stack_bottom:
    resb 16384      ; 16 KiB
stack_top:


section .text
global _start
_start:
    mov esp, stack_top
    call kernel_main
    cli
.hang:
    hlt
    jmp .hang
