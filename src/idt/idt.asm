section .asm

global_idt_load
idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp + 8]  ; first argument
    lidt [ebx]

    pop ebp
    ret
