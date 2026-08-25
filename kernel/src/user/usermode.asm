global user_jmp
user_jmp:
    push 0x23 ; SS
    push esp
    pushf
    pop eax
    or eax, 0x200
    push eax
    push 0x1B ; CS
    mov ecx, [esp+4] ; EIP (usermode entry code)
    push ecx
    iret

