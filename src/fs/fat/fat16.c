#include "fat16.h"
#include "status.h"
#include "string/string.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "kernel.h"


int fat16_resolve(struct disk* disk);
void* fat16_open(struct disk* disk, struct path_part* path, FILE_MODE mode);

struct filesystem  fat16_fs = {
    .resolve = fat16_resolve,
    .open = fat16_open
};

struct filesystem* fat16_init()
{
    strcpy(fat16_fs.name, "FAT16");
    return &fat16_fs;
}


static void fat16_init_private(struct disk* disk, struct fat_private* private)
{
    memset(private, 0, sizeof(struct fat_private));
    private->cluster_read_stream = disk_streamer_new(disk->id);
    private->fat_read_stream = disk_streamer_new(disk->id);
    private->directory_stream = disk_streamer_new(disk->id);
}


int fat16_sector_to_absolute(struct disk* disk, int sector)
{
    return sector * disk->sector_size;
}


int fat16_get_total_items_for_directory(struct disk* disk, uint32_t directory_start_sector)
{
    struct fat_directory_item item;
    struct fat_directory_item empty_item;
    memset(&empty_item, 0, sizeof(empty_item));

    struct fat_private* fat_private = disk->fs_private;
    int res = 0;
    int i = 0;
    int directory_start_pos = directory_start_sector * disk->sector_size;
    struct disk_stream* stream = fat_private->directory_stream;
    
    if (disk_streamer_seek(stream, directory_start_pos) != GUAVAOS_ALL_OK)
    {
        res = -EIO;
        goto out;
    }

    while(1)
    {
        if (disk_streamer_read(stream, &item, sizeof(item)) != GUAVAOS_ALL_OK)
        {
            res = -EIO;
            goto out;
        }

        if (item.filename[0] == 0x00)
        {
            break;
        }

        if (item.filename[0] == 0xE5)
        {
            continue;
        }
        i++;
    }

    res = i;
out:
    return res;
}


int fat16_get_root_directory(struct disk* disk, struct fat_private* fat_private, struct fat_directory *directory)
{
    int res = 0;
    struct fat_header* primary_header = &fat_private->header.primary_header;
    // absolute sector position of the start of the root directory
    int root_dir_sector_pos = (primary_header->fat_copies * primary_header->sectors_per_fat) + primary_header->reserved_sectors;
    int root_dir_entries = fat_private->header.primary_header.root_directory_entries;
    int root_dir_size = (root_dir_entries * sizeof(struct fat_directory_item));
    int total_sectors = root_dir_size / disk->sector_size;

    if (root_dir_size % disk->sector_size)
    {
        total_sectors += 1;
    }

    int total_items = fat16_get_total_items_for_directory(disk, root_dir_sector_pos);

    struct fat_directory_item* dir = kzalloc(root_dir_size);

    if (!dir)
    {
        res = -ENOMEM;
        goto out;
    }


    struct disk_stream* stream = fat_private->directory_stream;

    if (disk_streamer_seek(stream, fat16_sector_to_absolute(disk, root_dir_sector_pos)) != GUAVAOS_ALL_OK)
    {
        res = -EIO;
        goto out;
    }

    if (disk_streamer_read(stream, dir, root_dir_size) != GUAVAOS_ALL_OK)
    {
        res = -EIO;
        goto out;
    }


    directory->item = dir;
    directory->total = total_items;
    directory->sector_pos = root_dir_sector_pos;
    directory->ending_sector_pos = root_dir_sector_pos + (root_dir_size / disk->sector_size);

out:
    return res;
}


int fat16_resolve(struct disk* disk)
{
    int res = 0;
    struct fat_private* fat_private = kzalloc(sizeof(struct fat_private));
    fat16_init_private(disk, fat_private);

    // binding the disk to us
    disk->fs_private = fat_private;
    disk->filesystem = &fat16_fs;

    struct disk_stream* stream = disk_streamer_new(disk->id);

    
    if (!stream)
    {
        res = -ENOMEM;
        goto out;
    }

    if (disk_streamer_read(stream, &fat_private->header, sizeof(fat_private->header)) != GUAVAOS_ALL_OK)
    {
        res = -EIO;
        goto out;
    }

    if (fat_private->header.shared.extended_header.signature != 0x29)
    {
        res = -EFSNOTUS;
        goto out;
    }

    if (fat16_get_root_directory(disk, fat_private, &fat_private->root_directory) != GUAVAOS_ALL_OK)
    {
        res = -EIO;
        goto out;
    }


out:
    if (stream)
        disk_streamer_close(stream);


    if (res < 0)
    {   
        kfree(fat_private);
        disk->fs_private = 0;
    }
        

    return res;
}

struct fat_item* fat16_find_item_in_directory(struct disk* disk, struct fat_directory* directory, const char * name)
{
    struct fat_item* f_item = 0;
    char tmp_filename[GUAVAOS_MAX_PATH];

    for (int i = 0; i < directory->total; i++)
    {
        fat16_get_full_relative_filename(&directory->item[i], tmp_filename, sizeof(tmp_filename));
    }
}

struct fat_item* fat16_get_root_directory_entry(struct disk* disk, struct path_part* path)
{
    struct fat_private* fat_private = disk->fs_private;
    struct fat_item* curren_item = 0;
    struct fat_item* root_item = fat16_find_item_in_directory(disk, &fat_private->root_directory, path->part);

    if (!root_item)
    {
        goto out;
    }

out:
    return curren_item;
}   


void* fat16_open(struct disk* disk, struct path_part* path, FILE_MODE mode)
{
    if (mode != FILE_MODE_READ)
    {
        return ERROR(-ERDONLY);
    }

    struct fat_file_descriptor* descriptor = 0;
    descriptor = kzalloc(sizeof(struct fat_file_descriptor));
    if (!descriptor)
    {
        return ERROR(-ENOMEM);
    }

    descriptor->item = fat16_get_root_directory_entry(disk, path);
    if (!descriptor->item)
    {
        return ERROR(-EIO);
    }

    descriptor->pos = 0;

    return descriptor;
}