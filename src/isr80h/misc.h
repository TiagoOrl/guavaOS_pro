#ifndef H_ISR80h_MISC
#define H_ISR80h_MISC

#include "idt/idt.h"
#include "task/task.h"

void* isr80h_command0_sum(struct interrupt_frame * frame);


#endif