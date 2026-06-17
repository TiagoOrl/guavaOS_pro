#ifndef H_KEYBOARD
#define H_KEYBOARD

#include "task/process.h"
#include "task/task.h"
#include "status.h"
#include "kernel.h"
#include "ps2.h"


typedef int (*KEYBOARD_INIT_FN)();

struct keyboard
{
    KEYBOARD_INIT_FN init;
    char name[20];
    struct keyboard* next;
};

void keyboard_init();
void keyboard_backspace(struct process* process);
void keyboard_push(char c);
char keyboard_pop();
int keyboard_insert(struct keyboard* keyboard);

#endif