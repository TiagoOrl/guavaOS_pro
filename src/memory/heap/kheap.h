#ifndef H_KHEAP
#define H_KHEAP

#include <stdint.h>
#include <stddef.h>

void* kmalloc(size_t size);
void kfree(void* ptr);
void kheap_init();
void* kzalloc(size_t size);

#endif