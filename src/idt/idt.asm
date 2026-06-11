[BITS 32]

section .asm

extern int21h_handler
extern no_interrupt_handler
extern isr80h_handler

global idt_load
global int21h
global no_interrupt
global enable_interrupts
global disable_interrupts
global isr80h_wrapper

enable_interrupts:
    sti
    ret


disable_interrupts:
    cli
    ret


idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp + 8]  ; first argument
    lidt [ebx]

    pop ebp
    ret


int21h:
    cli
    pushad

    call int21h_handler

    popad
    sti
    iret



no_interrupt:
    cli
    pushad

    call no_interrupt_handler

    popad
    sti
    iret


isr80h_wrapper:
    ; pushed the general purpose registers to the stack
    pushad

    ; push the stack pointer so that we are pointing to the interrupt frame
    push esp
    push eax    ; eax holds our command
    call isr80h_handler
    mov dword[tmp_res], eax
    add esp, 8

    ; restore general purpose registers for user land
    popad
    mov eax, [tmp_res]
    iretd


section .data
; stores the return result from isr80h_handler
tmp_res: dd 0