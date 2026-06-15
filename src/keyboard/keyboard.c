#include "keyboard.h"
#include "status.h"
#include "kernel.h"


static struct keyboard* keyboard_list_head = 0;
static struct keyboard* keyboard_list_last = 0;



void keyboard_init()
{

}


int keyboard_insert(struct keyboard* keyboard)
{
    int res = 0;

    if (keyboard->init == 0)
    {
        res = -EINVARG;
        goto out;
    }

    if (keyboard_list_last)
    {
        keyboard_list_last->next = keyboard;
        keyboard_list_last = keyboard;
    }

    else 
    {
        keyboard_list_head = keyboard;
        keyboard_list_last = keyboard;
    }


    res = keyboard->init();

out:
    return res;
}


static int keyboard_get_tail_index(struct process* process)
{
    return process->keyboard.tail % GUAVAOS_KEYBOARD_BUFFER_SIZE;
}


void keyboard_backspace(struct process* process)
{
    process->keyboard.tail--;
    int reail_index = keyboard_get_tail_index(process);
    process->keyboard.buffer[reail_index] = 0;
}


void keyboard_push(char c)
{
    struct process* process = process_current();

    if (!process)
    {
        return;
    }

    int real_index = keyboard_get_tail_index(process);
    process->keyboard.buffer[real_index] = c;
    process->keyboard.tail++;
}


char keyboard_pop()
{
    if (!task_current())
        return 0;

    struct process* process = task_current()->process;
    int real_index = process->keyboard.head % GUAVAOS_KEYBOARD_BUFFER_SIZE;
    char c = process->keyboard.buffer[real_index];

    if (c == 0x00)
        return 0;

    process->keyboard.buffer[real_index] = 0;
    process->keyboard.head++;
    return c;
}