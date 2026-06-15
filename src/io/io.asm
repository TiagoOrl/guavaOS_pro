[BITS 32]

section .asm


; export routine
global insb 
global insw
global outb
global outw


insb:
    push ebp
    mov ebp, esp

    xor eax, eax ; zero the eax register
    mov edx, [ebp + 8] ; the port parameter
    in al, dx   ; set the lower bits of al to edx port value

    pop ebp
    ret

insw:
    push ebp
    mov ebp, esp

    xor eax, eax
    mov edx, [ebp + 8]
    in ax, dx

    pop ebp
    ret


outb:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 12]
    mov edx, [ebp + 8]
    out dx, al  ; output one byte to the specified port

    pop ebp
    ret


outw:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 12]
    mov edx, [ebp + 8]
    out dx, ax

    pop ebp 
    ret
