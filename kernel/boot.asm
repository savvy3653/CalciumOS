extern kernel_main
extern _kernel_start
extern _kernel_end

ALIGN_FLAG  equ 1<<0                   ; align loaded modules on page boundaries
MEMINFO     equ 1<<1                   ; provide memory map
FLAGS       equ ALIGN_FLAG | MEMINFO   ; this is the Multiboot 'flag' field
MAGIC       equ 0x1BADB002             ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)       ; checksum of above, to prove we are multiboot


section .multiboot.data
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bootstrap_stack nobits alloc noexec write
align 16
stack_bottom:
    resb 16384      ; 16 KiB
stack_top:


section .bss
align 4096
boot_page_directory:
    resb 4096
boot_page_table1:
    resb 4096


section .multiboot.text
global _start
_start:
    mov edi, (boot_page_table1 - 0xC0000000)
    mov esi, 0
    mov ecx, 1023

map_loop:
    cmp esi, _kernel_start
    jl skip_map

    cmp esi, (_kernel_end - 0xC0000000)
    jge done_map

    mov edx, esi
    or edx, 0x003
    mov [edi], edx

skip_map:
    add esi, 4096
    add edi, 4
    loop map_loop

done_map:
    mov dword [boot_page_table1 - 0xC0000000 + 1023 * 4], (0x000B8000 | 0x003)

    mov dword [boot_page_directory - 0xC0000000 + 0], (boot_page_table1 - 0xC0000000 + 0x003)
    mov dword [boot_page_directory - 0xC0000000 + 768 * 4], (boot_page_table1 - 0xC0000000 + 0x003)

    mov ecx, (boot_page_directory - 0xC0000000)
    mov cr3, ecx

    mov ecx, cr0
    or ecx, 0x80010000
    mov cr0, ecx

    lea ecx, [higher_half]
    jmp ecx

section .text
global hang
higher_half:
    mov dword [boot_page_directory + 0], 0

    mov ecx, cr3
    mov cr3, ecx

    mov esp, stack_top

    call kernel_main
    cli
hang:
    hlt
    jmp hang
