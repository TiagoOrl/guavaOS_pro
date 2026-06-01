#include "./kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
#include "string/string.h"
#include "fs/pparser.h"
#include "./disk/streamer.h"
#include "./fs/file.h"

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


void kernel_main()
{
    terminal_initialize();
    const char* msg = "Hello World\n test\n\n";
    print(msg, 6);


    // initialize the heap
    kheap_init();

    // init the filesystem
    fs_init();

    // search and initialize the disks
    disk_search_and_init();

    // // initialize the interrupt descriptor table
    idt_init();

    // setup paging
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    // switch to kernel paging chunk
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));
    enable_paging();


    // example: reading a sector from disk with the ata driver
    // char buffer[512];
    // disk_read_sector(0, 1, buffer);


    enable_interrupts(); 

    // test disk streaming
    // struct disk_stream *stream = disk_streamer_new(0);
    // disk_streamer_seek(stream, 0x2200);

    // unsigned char c = 0;
    // disk_streamer_read(stream, &c, 1);


    int fd = fopen("0:/hello.txt", "r");

    if (fd)
    {
        // print("we opened hello.txt\n", 3);
        // char buff[17];
        // buff[16] = 0x00;
        // fseek(fd, 2, SEEK_SET);
        // fread(buff, 14, 1, fd);
        // print(buff, 4);

        struct file_stat s;
        fstat(fd, &s);
        fclose(fd);
        print("test", 3);
    }

    while (1)
    {

    }


    // // call asm function
    // problem();
}