#ifndef TPE_STRING_H
#define TPE_STRING_H

int strcmp(const char *str1, const char *str2);
int strlen(const char * str);
char* strcat(char* destination, const char* source);
void* memcpy(void* dest, const void* src, int n);
int charcount(const char* str, char c);

#endif //TPE_STRING_H
