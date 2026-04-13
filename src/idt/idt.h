#ifndef H_IDT
#define H_IDT
#include <stdint.h>


struct idt_desc
{
    uint16_t offset_1;  // offset bits 0 - 15
    uint16_t selector;  // selector in out GDT
    uint8_t zero;   // does nothing, unused set to zero
    uint8_t type_attr;  // Descriptor type and attributes

} __attribute__((packed));


#endif