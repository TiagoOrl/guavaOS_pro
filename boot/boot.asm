org 0x7c00
bits 16


start:
    ; reset register values
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax

    mov sp, 0x7C00 ; moves the stack pointer to the start of the program OS
    
    call print_msg
    jmp $


print_msg:
    mov si, msg
    mov bx, 0   ; page number
.loop:
    lodsb   ; loads the next byte from the si register into AL register
    cmp al, 0   ; checks if si reached the end of the string
    je .done
    
    mov ah, 0eh ; code to print a char to the screen
    int 0x10 ; video interrupt: checks that 0x0E is in ah and prints char stored in AL to the screen
    jmp .loop
.done:
    ret

msg: db 'Welcome to Guava OS v0.0.1', 0
times 510-($ - $$) db 0
dw 0xAA55