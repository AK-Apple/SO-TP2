#ifndef TPE_COMMAND_H
#define TPE_COMMAND_H

typedef struct {
    char * title;
    char * desc;
    void (*command)();
} command;

void print_time();
void print_help();

void clear();
void divideByZero();
void invalidOpcode();
void time();
void changeSize_1();
void changeSize_2();
void changeSize_3();
void changeSize_4();
void changeSize_5();

//para excepciones (buscan en Assembler)
extern void runInvalidOpcodeException(void);
extern void runZeroDivisionException(void);

#endif //TPE_COMMAND_H