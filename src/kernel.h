#ifndef H_KERNEL
#define H_KERNEL

#define VGA_WIDTH 80
#define VGA_HEIGHT 20
#define GUAVAOS_MAX_PATH 108
void kernel_main();
void print(const char * msg, char colour);
void panic(const char* msg);


#define ERROR(value) (void*) (value)
#define ERROR_I(value) (int)(value)
#define ISERR(value) ((int)value < 0)

#endif