#include "ps2.h"

struct keyboard ps2_keyboard = {
    .name = {"PS2"},
    .init = ps2_keyboard_init
};


int ps2_keyboard_init()
{
    // 0x64 = command register, 0xAE = command byte: enables first PS2 port
    outb(PS2_PORT, PS2_COMMAND_ENABLE_FIRST_PORT);
    return 0;
}


struct keyboard* ps2_get()
{
    return &ps2_keyboard;
}


void classic_keyboard_handle_interrupt()
{


}


uint8_t ps2_scancode_to_char(uint8_t scancode)
{
    size_t sizeof_keyboard_set_one = sizeof(keyboard_scan_set_one) / sizeof(uint8_t);
    if (scancode > sizeof_keyboard_set_one)
    {
        return 0;
    }

    char c = keyboard_scan_set_one[scancode];
    return c;
}