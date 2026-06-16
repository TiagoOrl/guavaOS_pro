#include "./kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "memory/memory.h"
#include "disk/disk.h"
#include "string/string.h"
#include "fs/pparser.h"
#include "./disk/streamer.h"
#include "./fs/file.h"
#include "gdt/gdt.h"
#include "config.h"
#include "./task/tss.h"
#include "./task/task.h"
#include "./task/process.h"
#include "./status.h"
#include "isr80h/isr80h.h"
#include "keyboard/keyboard.h"


uint16_t * video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

uint16_t terminal_make_char(char c, char colour)
{
    return (colour << 8 ) | c; // most 8 bits left = colour, rightmost 8 bits = char
}


void terminal_putchar(int x, int y, char c, char colour)
{
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, colour);
}


void terminal_writechar(char c, char colour)
{
    if (c == '\n')
    {
        terminal_row += 1;
        terminal_col = 0;
        return;
    }

    terminal_putchar(terminal_col, terminal_row, c, colour);
    terminal_col += 1;
    if (terminal_col >= VGA_WIDTH)
    {
        terminal_col = 0;
        terminal_row++;
    }
    
}


void terminal_initialize()
{
    terminal_row = 0;
    terminal_col = 0;
    video_mem = (uint16_t*)(0xB8000);    // video memory location

    for (int y = 0;y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            // clears the display with blank blank characters
            terminal_putchar(x, y, ' ', 0);
        }
    }
}



void print(const char * msg, char colour)
{
    size_t len = strlen(msg);
    for (int i = 0; i < len; i++)
    {
        terminal_writechar(msg[i], colour);
    }
}


extern void problem();

static struct paging_4gb_chunk* kernel_chunk = 0;


void panic(const char* msg)
{
    print(msg, 10);
    while(1)
    {

    }
}

struct tss tss;
struct gdt gdt_real[GUAVAOS_TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[GUAVAOS_TOTAL_GDT_SEGMENTS] = {
    { .base = 0x00, .limit = 0x00, .type =  0x00 },   // NULL segment
    { .base = 0x00, .limit = 0xFFFFFFFF, .type =  0x9a },   // kernel code segment
    { .base = 0x00, .limit = 0xFFFFFFFF, .type =  0x92 },    // kernel data segment
    { .base = 0x00, .limit = 0xFFFFFFFF, .type = 0xf8 },    // user code segment
    { .base = 0x00, .limit = 0xFFFFFFFF, .type = 0xf2 },     // user data segment
    { .base = (uint32_t)&tss, .limit = sizeof(tss), .type = 0xe9 }  // TSS segment 
};


void kernel_page()
{
    kernel_registers();
    paging_switch(kernel_chunk);
}

void pic_timer_callback(struct interrupt_frame * frame)
{
    print("Timer activated\n", 15);
}


void kernel_main()
{
    terminal_initialize();


    memset(gdt_real, 0x00, sizeof(gdt_real));
    gdt_structured_to_gdt(gdt_real, gdt_structured, GUAVAOS_TOTAL_GDT_SEGMENTS);

    // load GDT
    gdt_load(gdt_real, sizeof(gdt_real));

    // initialize the heap
    kheap_init();

    // init the filesystem
    fs_init();

    // search and initialize the disks
    disk_search_and_init();

    // initialize the interrupt descriptor table
    idt_init();

    // setup TSS
    memset(&tss, 0x00, sizeof(tss));
    tss.esp0 = 0x600000;
    tss.ss0 = KERNEL_DATA_SELECTOR;


    // load TSS
    tss_load(0x28);

    // setup paging
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    // switch to kernel paging chunk
    paging_switch(kernel_chunk);
    enable_paging();


    // register kernel commands
    isr80h_register_commands();

    keyboard_init();

    // idt_register_interrupt_callback(0x20, pic_timer_callback);


    // example: reading a sector from disk with the ata driver
    // char buffer[512];
    // disk_read_sector(0, 1, buffer);


    // enable_interrupts(); 

    // test disk streaming
    // struct disk_stream *stream = disk_streamer_new(0);
    // disk_streamer_seek(stream, 0x2200);

    // unsigned char c = 0;
    // disk_streamer_read(stream, &c, 1);


    struct process* process = 0;
    int res = process_load_switch("0:/blank.bin", &process);

    if (res != GUAVAOS_ALL_OK)
    {
        panic("failed to load blank.bin\n");
    }

    keyboard_push('A');

    task_run_first_ever_task();

    while (1){}


    // // call asm function
    // problem();
}