#include "io/io.h"
#include "disk.h"
#include "memory/memory.h"
#include "config.h"
#include "status.h"



struct disk disk;


// lba = logical block address to read from; total = total number of block to read; buffer = the buffer to read into
int disk_read_sector(int lba, int total, void* buffer)
{
    outb(0x1F6, lba >> 24 | 0xE0);
    outb(0x1f2, total);
    outb(0x1F3, (unsigned char)(lba & 0xff));
    outb(0x1f4, (unsigned char)(lba >> 8));
    outb(0x1f5, (unsigned char)(lba >> 16));
    outb(0x1f7, 0x20);


    unsigned short* ptr = (unsigned short*) buffer;

    for (int i = 0; i < total; i++)
    {
        // wait for the buffer to be ready
        char c = insb(0x1F7);

        while(!(c & 0x08))
        {
            c = insb(0x1f7);
        }

        // ready to read from the ATA controller, copy from hard disk to memory
        for (int j = 0; j < 256; j++)
        {
            *ptr = insw(0x1f0); // reads to bytes (a word) into our buffer
            ptr++;  // advance memory position to read into next iteration
        }
    }
 
    return 0;
}


void disk_search_and_init()
{
    memset(&disk, 0, sizeof(disk));
    disk.type = GUAVAOS_DISK_TYPE_REAL;
    disk.sector_size = GUAVAOS_SECTOR_SIZE;
    disk.id = 0;
    disk.filesystem = fs_resolve(&disk);
}


struct disk* disk_get(int index)
{
    if(index != 0)
        return 0;

    return &disk;
}


int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buffer)
{
    if (idisk != &disk)
    {
        return -EIO;
    }

    return disk_read_sector(lba, total, buffer);
}