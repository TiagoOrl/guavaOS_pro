#include "./kernel.h"
#include <stdint.h>
#include <stddef.h>

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



void print_string_video(const char * msg, unsigned int size, char colour)
{
    
    for (int i = 0; i < size; i++)
    {
        terminal_writechar(msg[i], colour);
    }
}


size_t strlen(const char * msg)
{
    size_t s = 0;
    while(msg[s] != 0)
    {
        s++;
    }

    return s;
}



void kernel_main()
{
    terminal_initialize();
    const char* msg = "Hello World";
    print_string_video(msg, strlen(msg), 6);
}