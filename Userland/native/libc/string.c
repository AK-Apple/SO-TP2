// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com


#include "../include/string.h"

int strcmp(const char *str1, const char *str2) 
{
    while (*str1 && (*str1 == *str2)) 
    {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}

int is_whitespace(char c) 
{
    return c == '\n' || c == ' ' || c == '\t';
}

int is_digit(char c) 
{
    return c >= '0' && c <= '9'; 
}

int is_integer(char *str) 
{
    while(*str) {
        if(!is_digit(*str)) return 0;
        str++;
    }
    return 1;
}

int trim_end(char* str, char c) 
{
    int i = 0;
    if (!str[i]) return 0;
    for (i = 0; str[i]; i++);
    i--;
    while (i > 0 && is_whitespace(str[i])) i--;

    if (str[i] == c) 
    {
        str[i] = 0;
        return 1;
    }
    return 0;
}

void compact_whitespace(char *str) 
{
    int j = 0;
    for(int i = 0; str[i]; i++) 
    {
        if((i > 0 && !is_whitespace(str[i-1])) || !is_whitespace(str[i]))
            str[j++] = str[i];
    }
    str[j] = 0;
}

char* strcat(char* destination, const char* source) 
{
    // Find the end of the destination string
    char* ptr = destination + strlen(destination);

    // Append the source string to the destination string
    while (*source != '\0') 
    {
        *ptr++ = *source++;
    }

    // Null-terminate the resulting string
    *ptr = '\0';

    return destination;
}

int strlen(const char* str) 
{
    int len = 0;
    while (*str != '\0') 
    {
        len++;
        str++;
    }
    return len;
}

void* memcpy(void* dest, const void* src, int n) 
{
    char* d = (char*)dest;
    const char* s = (const char*)src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

int charcount(const char* str, char c) 
{
	int count = 0;
	for(int i = 0; str[i]; i++)
    {
		if(str[i] == c)
        {
			count++;
		}
	}

	return count;
}

int char_to_upper(int c) 
{
    const char OFFSET = 'a' - 'A';
    return (c >= 'a' && c <= 'z') ? c - OFFSET : c;
}

int char_to_lower(int c) 
{
    const char OFFSET = 'a' - 'A';
    return (c >= 'A' && c <= 'Z') ? c + OFFSET : c;
}

int is_vowel(int c) 
{
    c = char_to_lower(c);
    switch(c) {
    case 'a':
    case 'e':
    case 'i':
    case 'o':
    case 'u':
        return 1;
    default:
        return 0;
    }
}