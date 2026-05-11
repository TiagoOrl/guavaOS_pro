#ifndef H_DISK
#define H_DISK
 
#include "fs/file.h"

typedef unsigned int GUAVAOS_DISK_TYPE;

// represents a real, physical hard disk
#define GUAVAOS_DISK_TYPE_REAL 0


struct disk {
    GUAVAOS_DISK_TYPE type;
    int sector_size;
    
    // filesystem binded to the disk
    struct filesystem* filesystem;
};


int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buffer);struct disk* disk_get(int index);
struct disk* disk_get(int index);
void disk_search_and_init();



#endif