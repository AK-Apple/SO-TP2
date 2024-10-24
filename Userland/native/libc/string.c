#include "../include/string.h"

int strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}

int is_whitespace(char c) {
    return c == '\n' || c == ' ' || c == '\t';
}

int trim_end(char* str, char c) {
    int i = 0;
    if (!str[i]) return 0;
    for (i = 0; str[i]; i++);
    i--;
    while (i > 0 && is_whitespace(str[i])) i--;

    if (str[i] == c) {
        str[i] = 0;
        return 1;
    }
    return 0;
}

void compact_whitespace(char *str) {
    int j = 0;
    for(int i = 0; str[i]; i++) {
        if(!is_whitespace(str[i]) || (i > 0 && !is_whitespace(str[i-1])))
            str[j++] = str[i];
    }
    str[j] = 0;
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

int charcount(const char* str, char c) {
	int count = 0;
	for(int i = 0; str[i]; i++){
		if(str[i] == c){
			count++;
		}
	}

	return count;
}