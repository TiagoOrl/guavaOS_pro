#ifndef H_STRING
#define H_STRING

#include <stdbool.h>

int strlen(const char* ptr);
int strnlen(const char* ptr, int max);
int to_numeric_digit(char c);
bool is_digit(char c);
char* strcpy(char* dest, const char* src);
char tolower(char s1);
int strncmp(const char* str1, const char* str2, int n);
int istrcmp(const char* s1, const char* s2, int n);
int strnlen_terminator(const char* str, int max, char terminator);

#endif