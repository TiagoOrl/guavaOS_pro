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

// replace spaces with null bytes
void fat16_to_proper_string(char** out, const char* in)
{

    while(*in != 0x00 && *in != 0x20)
    {
        **out = *in;
        *out += 1;
        in += 1;
    }

    if (*in == 0x20)
    {
        **out = 0x00;
    }
}

void fat16_get_full_relative_filename(struct fat_directory_item* item, char* out, int max_len)
{
    memset(out, 0x00, max_len);
    char* out_tmp = out;
    fat16_to_proper_string(&out_tmp, (const char*)item->filename);

    if (item->ext[0] != 0x00 && item->ext[0] != 0x20)
    {
        *out_tmp++ = '.';
        fat16_to_proper_string(&out_tmp, (const char*)item->ext);
    }
}


struct fat_directory_item* fat16_clone_directory_item(struct fat_directory_item* item, int size)
{
    struct fat_directory_item *item_copy = 0;
    if (size < sizeof(struct fat_directory_item))
    {
        goto out;
    }

    item_copy = kzalloc(size);

    if (!item_copy)
    {
        goto out;
    }

    memcpy(item_copy, item, size);

out:
    return item_copy;
}


struct fat_directory* fat16_load_fat_directory(struct disk *disk, struct fat_directory_item* item)
{
    int res = 0;
    struct fat_directory* directory = 0;
    struct fat_private* fat_private = disk->fs_private;


    // if its a file
    if (!(item->attribute & FAT_FILE_SUBDIRECTORY))
    {
        res = -EINVARG;
        goto out;
    }

    directory = kzalloc(sizeof(struct fat_directory));
    if (!directory)
    {
        res = -ENOMEM;
        goto out;
    }


    int cluster = fat16_get_first_cluster(item);
    int cluster_sector = fat16_cluster_to_sector(fat_private, cluster);
    int total_items = fat16_get_total_items_for_directory(disk, cluster_sector);
    directory->total = total_items;
    int directory_size = directory->total * sizeof(struct fat_directory_item);

    directory->item = kzalloc(directory_size);  // making room in memory to load the entire directory from disk into memory

    if(!directory->item)
    {
        res = -ENOMEM;
        goto out;
    }

    res = fat16_read_internal(disk, cluster, 0x00, directory_size, directory->item);

    if (res != GUAVAOS_ALL_OK)
    {
        goto out;
    }


out:
    if (res != GUAVAOS_ALL_OK)
    {
        fat16_free_directory(directory);
    }
}


struct fat_item* fat16_new_fat_item_for_directory_item(struct disk* disk, struct fat_directory_item *item)
{
    struct fat_item* f_item = kzalloc(sizeof(struct fat_item));
    if (!f_item)
    {
        return 0;
    }

    if (item->attribute & FAT_FILE_SUBDIRECTORY)
    {
        f_item->directory = fat16_load_fat_directory(disk, item);
        f_item->type = FAT_ITEM_TYPE_DIRECTORY;
    }

    f_item->type = FAT_ITEM_TYPE_FILE;
    f_item->item = fat16_clone_directory_item(item, sizeof(struct fat_directory_item));

    return f_item;

}


struct fat_item* fat16_find_item_in_directory(struct disk* disk, struct fat_directory* directory, const char * name)
{
    struct fat_item* f_item = 0;
    char tmp_filename[GUAVAOS_MAX_PATH];

    for (int i = 0; i < directory->total; i++)
    {
        fat16_get_full_relative_filename(&directory->item[i], tmp_filename, sizeof(tmp_filename));
        if (strncmp(tmp_filename, name, sizeof(tmp_filename)) == 0)
        {
            // found id, lets create a new fat_item
            f_item = fat16_new_fat_item_for_directory_item(disk, &directory->item[i]);
        }
    }

    return f_item;
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