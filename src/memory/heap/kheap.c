#include "kheap.h"
#include "heap.h"
#include "./config.h"
#include "kernel.h"


// 1024*1024 = 1048576 = 1MB
// 1048576 * 100 = 100MB
// 4096 = heap block size
// 104857600 / 4096 = 25600 kernel heap entries


struct heap kernel_heap;
struct heap_table kernel_heap_table;


void kheap_init()
{
    int total_table_entries = GUAVAOS_HEAP_SIZE_BYTES / GUAVAOS_HEAP_BLOCK_SIZE;
    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY*) GUAVAOS_HEAP_TABLE_ADDRESS;
    kernel_heap_table.total = total_table_entries;


    void* end = (void*)(GUAVAOS_HEAP_ADDRESS + GUAVAOS_HEAP_SIZE_BYTES);
    int res = heap_create(&kernel_heap, (void*)(GUAVAOS_HEAP_ADDRESS), end, &kernel_heap_table);

    if (res < 0)
    {
        print("Failed to create heap/n", 5);
    }
}