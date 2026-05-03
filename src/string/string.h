#ifndef H_STRING
#define H_STRING

#include <stdbool.h>

int strlen(const char* ptr);
int strnlen(const char* ptr, int max);
int to_numeric_digit(char c);
bool is_digit(char c);

#endif