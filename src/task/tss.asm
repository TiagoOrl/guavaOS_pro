[BITS 32]

section .asm


global tss_load

; Loads the Task Switch Segment
tss_load:
    push ebp
    mov ebp, esp
    mov ax, [ebp + 8]   ; TSS segment
    ltr ax
    pop ebp
    ret
