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
};

#endif