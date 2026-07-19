extern kernel_main
; Declare constants for the multiboot header.
ALIGN_FLAG  equ 1<<0                   ; align loaded modules on page boundaries
MEMINFO     equ 1<<1                   ; provide memory map
FLAGS       equ ALIGN_FLAG | MEMINFO   ; this is the Multiboot 'flag' field
MAGIC       equ 0x1BADB002             ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)       ; checksum of above, to prove we are multiboot

; Declare a multiboot header that marks the program as a kernel. These are
; magic values documented in the multiboot standard. The bootloader will
; search for this signature in the first 8 KiB of the kernel file, aligned at
; a 32-bit boundary. The signature is in its own section so the header can be
; forced to be within the first 8 KiB of the kernel file.
section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

; The multiboot standard does not define the value of the stack pointer
; register (esp), and it is up to the kernel to provide a stack. This
; allocates room for a small stack by creating a symbol at the bottom, then
; reserving 16384 bytes for it, and finally a symbol at the top. The stack
; grows downwards on x86. It's in its own section (nobits/BSS) so the kernel
; file stays smaller, since it does not contain an uninitialized stack.
section .bss
align 16
stack_bottom:
    resb 16384      ; 16 KiB
stack_top:
; The linker script specifies _start as the entry point to the kernel, and
; the bootloader will jump to this position once the kernel has been loaded.
; It doesn't make sense to return from this function, as the bootloader is
; gone.
section .text
global _start
_start:
    ; The bootloader has loaded us into 32-bit protected mode on an x86
    ; machine. Interrupts are disabled. Paging is disabled. The processor
    ; state is as defined in the multiboot standard. The kernel has full
    ; control of the CPU. There's no printf function unless the kernel
    ; provides its own. There are no safeguards, only what the kernel
    ; provides itself.

    ; To set up a stack, point esp at the top of the stack (it grows
    ; downwards on x86). This must be done in assembly, since languages like
    ; C cannot function without a stack.
    mov esp, stack_top

    ; Good place to initialize crucial processor state before the
    ; high-level kernel is entered: GDT, paging, etc. should go here.

    ; Enter the high-level kernel. The ABI requires the stack be 16-byte
    ; aligned at the time of the call instruction (which then pushes a
    ; 4-byte return address). The stack was 16-byte aligned above and
    ; nothing has been pushed since, so alignment is preserved.
    call kernel_main

    ; If the system has nothing more to do, put the computer into an
    ; infinite loop:
    ; 1) cli - disable interrupts (already disabled by bootloader, but
    ;    kept here in case kernel_main re-enabled them and returned).
    ; 2) hlt - wait for the next interrupt; since interrupts are disabled,
    ;    this locks up the CPU.
    ; 3) jmp back to hlt in case of NMI or SMM wakeup.
    cli
.hang:
    hlt
    jmp .hang

; nasm doesn't need an explicit .size directive like GAS — symbol sizes are
; inferred from ELF section layout, so it's simply omitted.