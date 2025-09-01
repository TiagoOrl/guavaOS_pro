org 0
bits 16

_start:
    jmp short start
    nop

times 33 db 0

start:
    jmp 0x7c0:step2


step2:
    cli ; clear interrupts
    mov ax, 0x7c0
    mov ds, ax
    mov es, ax
    mov ax, 0x00
    mov ss, ax ; set stack segment to 0
    mov sp, 0x7C00 ; moves the stack pointer to the start of the program OS

    sti ; enables interrupts

    mov si, msg
    call print_msg

    call read_sector

    mov si, buffer
    call print_msg

    jmp $


; AH = 02h
; AL = number of sectors to read (must be nonzero)
; CH = low eight bits of cylinder number
; CL = sector number 1-63 (bits 0-5)
; high two bits of cylinder (bits 6-7, hard disk only)
; DH = head number
; DL = drive number (bit 7 set for hard disk)
; ES:BX -> data buffer

; Return:
; CF set on error
; if AH = 11h (corrected ECC error), AL = burst length
; CF clear if successful
; AH = status (see #00234)
; AL = number of sectors transferred (only valid if CF set for some
; BIOSes)
read_sector:
    mov ah, 2 ; read sector command
    mov al, 1 ; one sector to read
    mov ch, 0 ; cylinder number low eight bits
    mov cl, 2 ; read sector 2 (location of message)
    mov dh, 0 ;  head number  = 0
    mov bx, buffer ; point to buffer location
    int 0x13 ; disk read sectors into memory interrupt
    jc read_sector_error
    ret


read_sector_error:
    mov si, err_read_msg
    call print_msg


print_msg:
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

msg: db 'Welcome to Guava OS v0.0.1', 0x0D, 0x0A, 0x0D, 0x0A, 0
err_read_msg: db 'Failed to load sector from disk', 0x0D, 0x0A, 0
times 510-($ - $$) db 0
dw 0xAA55

buffer: