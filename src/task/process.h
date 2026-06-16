#ifndef H_PROCESS
#define H_PROCESS

#include <stdint.h>
#include "config.h"
#include <status.h>
#include "task.h"


struct process 
{
    uint16_t id;
    char filename[GUAVAOS_MAX_PATH];

    // main process task
    struct task* task;

    // the memory allocations of the process
    void* allocations[GUAVAOS_MAX_PROGRAM_ALLOCATIONS];

    // physical pointer to the process memory
    void* ptr;

    // physical pointer to the stack memory
    void* stack;

    // size of data pointed to by ptr
    uint32_t size;

    struct keyboard_buffer
    {   
        char buffer[GUAVAOS_KEYBOARD_BUFFER_SIZE];
        int tail;
        int head;
    } keyboard;
};

int process_load_for_slot(const char* filename, struct process** process, int process_slot);
int process_load(const char* filename, struct process** process);
struct process* process_current();
struct process* process_get(int process_id);
int process_load_switch(const char* filename, struct process** process);
int process_switch(struct process* process);

#endif