org 0x7c00
bits 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

_start:
    jmp short start
    nop

 times 33 db 0

start:
    jmp 0:step2


step2:
    cli ; clear interrupts
    mov ax, 0x00
    mov ds, ax
    mov es, ax
    mov ss, ax ; set stack segment to 0
    mov sp, 0x7c00 ; moves the stack pointer to the start of the program OS

    sti ; enables interrupts

    mov si, msg
    call print_msg

.load_protected:
    cli
    lgdt[gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:load32
    

;GDT
gdt_start:
gdt_null:
    ; fill 64 bits with zeroes
    dd 0x0
    dd 0x0

; offset 0x8
gdt_code:     ; CS should point to this
    dw 0xffff ; segment limit first 0-15 bits
    dw 0      ; base first 0-15 bits
    db 0      ; base 16-23 bits
    db 0x9a   ; access byte
    db 11001111b ; high 4 bit flag and low 4 bit flag
    db 0        ; base 24-31 bits

; offset 0x10
gdt_data:   ; DS, SS, ES, FS, GS
    dw 0xffff ; segment limit first 0-15 bits
    dw 0      ; base first 0-15 bits
    db 0      ; base 16-23 bits
    db 0x92   ; access byte
    db 11001111b ; high 4 bit flag and low 4 bit flag
    db 0        ; base 24-31 bits

gdt_end:


gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start




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
buffer:




[BITS 32]
load32:
    mov eax, 1  ; load from sector 1
    mov ecx, 100    ; number of sectors to load = 1
    mov edi, 0x0100000  ; the address we want to load the kernel into
    call ata_lba_read
    jmp CODE_SEG:0x0100000


ata_lba_read:
    mov ebx, eax ; backup the LBA
    ; send the highest 8 bits of the lba to the hard disk
    shr eax, 24
    or eax, 0xE0    ; selects the master drive
    mov dx, 0x1f6   ; port to write bits to
    out dx, al
    ; fininshed sending the highest 8 bits of the lba

    ; send the total sectors to read
    mov eax, ecx
    mov dx, 0x1f2
    out dx, al


    ; send more bits of the LBA
    mov eax, ebx ; restore the backup LBA
    mov dx, 0x1f3
    out dx, al
    ; end

    ; send more bits of the LBA
    mov dx, 0x1f4
    mov eax, ebx    ; restore the backup LBA
    shr eax, 8
    out dx, al
    ; end


    ; send the uppper 16 bits of the LBA
    mov dx, 0x1f5
    mov eax, ebx    ; restore the backup LBA
    shr eax, 16
    out dx, al
    ; end


    mov dx, 0x1f7
    mov al, 0x20
    out dx, al


    ; read all sectors into memory
.next_sector:
    push ecx

; check if we need to read
.try_again:
    mov dx, 0x1f7
    in al, dx
    test al, 8
    jz .try_again


; we need to read 256 words at a time
    mov ecx, 256
    mov dx, 0x1f0
    rep insw
    pop ecx
    loop .next_sector

    ; end of reading sectors into memory
    ret

times 510-($ - $$) db 0
dw 0xAA55




