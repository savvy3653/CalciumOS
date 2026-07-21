global idt_flush
extern pidt

idt_flush:
    mov eax, [esp+4]
    lidt [eax]
    ret