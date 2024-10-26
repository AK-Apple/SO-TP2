#include "../include/stdlib.h"
#include "../include/syscalls.h"
#include "../include/string.h"
#include "../include/stdio.h"

uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

uint64_t getchar() {
    char buf[1];
    sys_read(0, buf, 1);
    return buf[0];
}


uint64_t gets(char * buf, uint64_t length) {
    char c;
    int i = 0;
    do {
        c=getchar();
        if (c >= 0x20 && c <= 0x7F) {
            *buf = c;
            putchar(c);
        } else if (c == '\n') {
            *buf = '\0';
            putchar('\n');
            return i;
        }
        buf++;
    } while (i < length-1 && c != '\n');
    *buf = '\0';
    return i;
}

uint64_t atoi(char * str) { // ascii to integer
    uint64_t res = 0;
    int length = strlen(str);

    for (int i = 0; i < length; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return 0;
        }
        res = res * 10 + (str[i] - '0');
    }
    return res;
}

uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base) {
    char *p = buffer;
    char *p1, *p2;
    uint32_t digits = 0;

    do
    {
        uint32_t remainder = value % base;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
        digits++;
    }
    while (value /= base);

    *p = 0;

    p1 = buffer;
    p2 = p - 1;
    while (p1 < p2)
    {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }

    return digits;
}

char* itoa(int value, char* result, int base) {
    if (base < 2 || base > 36) { 
        *result = '\0'; return result; 
    }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
  
    // Reverse the string
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y){
    sys_putPixel(hexColor, x, y);
}

void putSquare(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t size) {
    for(int i=0; i<size; i++) {
        for (int j=0; j<size; j++) {
            putPixel(hexColor, x+j, y+i);
        }
    }
}

char getKey() {
    char c[1];
    if(sys_read(0, c, 1) <= 0){
        c[0] = 0;
    }
    return c[0];
}