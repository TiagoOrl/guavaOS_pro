org 0x7c00
bits 16


start:
    call print_msg
    jmp $


print_msg:
    mov si, msg
    mov bx, 0   ; page number
.loop:
    lodsb   ; loads single byte from si into al register
    cmp al, 0   ; checks if si reached the end of the string
    je .done
    ; BIOS interrupt to print a char into the screen
    mov ah, 0eh 
    int 0x10
    jmp .loop
.done:
    ret

msg: db 'Welcome to Guava OS v0.0.1', 0
times 510-($ - $$) db 0
dw 0xAA55