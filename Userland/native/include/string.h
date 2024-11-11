#ifndef TPE_STRING_H
#define TPE_STRING_H

int strcmp(const char *str1, const char *str2);
int strlen(const char * str);
char* strcat(char* destination, const char* source);
void* memcpy(void* dest, const void* src, int n);
int charcount(const char* str, char c);
int trim_end(char* str, char c);
int is_whitespace(char c);
void compact_whitespace(char *str);
int char_to_upper(int c);
int char_to_lower(int c);
int is_vowel(int c);
int is_digit(char c);
int is_integer(char *str);

#endif //TPE_STRING_H
