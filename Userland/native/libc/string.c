#include "../include/string.h"

int strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}

char* strcat(char* destination, const char* source) {
    // Find the end of the destination string
    char* ptr = destination + strlen(destination);

    // Append the source string to the destination string
    while (*source != '\0') {
        *ptr++ = *source++;
    }

    // Null-terminate the resulting string
    *ptr = '\0';

    return destination;
}

int strlen(const char * str) {
    int len = 0;
    while (*str != '\0') {
        len++;
        str++;
    }
    return len;
}

void* memcpy(void* dest, const void* src, int n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}