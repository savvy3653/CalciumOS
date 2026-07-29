
global set_page_dir
set_page_dir:
    push ebp
    mov ebp, esp
    mov ecx, [esp+8]
    mov cr3, ecx
    mov esp, ebp
    pop ebp
    ret