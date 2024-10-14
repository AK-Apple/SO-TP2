#include "../include/command.h"
#include "../include/syscalls.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/eliminator.h"

#define REG_SIZE 17
#define SECONDS 0
#define MINUTES 2
#define HOURS 4
#define DAY_OF_THE_WEEK 6
#define DAY_OF_THE_MONTH 7
#define MONTH 8
#define YEAR 9

static command commands[] = {
        {"help            :  ", "Muestra la lista de comandos.", print_help},
        {"time            :  ", "Muestra la hora.", time},
        {"eliminator      :  ", "Ejecuta el juego eliminator.", eliminator},
        {"size_<1-5>      :  ", "cambia tamanio de letra (entre 1 a 5).", changeSize_1},
        {"dividebyzero    :  ", "Muestra excepcion de division de 0.", divideByZero},
        {"invalidopcode   :  ", "Muestra excepcion de codigo invalido.", invalidOpcode},
        {"inforeg         :  ", "Muestra los registros guardados.", sys_getRegs},
        {"clear           :  ", "Limpia toda la pantalla.", clear},
        {"ps              :  ", "Lista la informacion de los procesos", print_process_state},
        {"kill            :  ", "mata un proceso dado un pid", kill_process},
        {"testp           :  ", "ejecuta test de proceso", test_processes_cmd},
};

void print_help() {
    printf("Presiona left alt para guardar registros en cualquier momento\n");
    printf("Comandos disponibles:\n");
    for (int i = 0 ; i < sizeof(commands)/sizeof(command) ; i++) {
        printf(commands[i].title);
        printf(" : ");
        printf(commands[i].desc);
        printf("\n");
    }
}


void clear() {
    sys_clear();
}

void divideByZero() {
    runZeroDivisionException();
}

void invalidOpcode() {
    runInvalidOpcodeException();
}

void changeSize_1() {
    sys_new_size(1);
}
void changeSize_2() {
    sys_new_size(2);
}
void changeSize_3() {
    sys_new_size(3);
}
void changeSize_4() {
    sys_new_size(4);
}
void changeSize_5() {
    sys_new_size(5);
}

void print_process_state() {
    sys_print_all_processes();
}

void kill_process(int pid) {
    sys_kill_process(pid);
}
int64_t test_processes_cmd(uint64_t argc, char *argv[]) {
    sys_create_process(test_processes, argc, argv);
}
void test_prio_cmd() {
    sys_create_process(test_prio, 0, 0);
}

void time() {

    int hours = sys_time(HOURS);
    // hours -= 3;          // el tema con pasar a horario de argentina es que habría que cambiar el día (y hasta el año si es 31/12) también, y es todito un lío
    // if(hours <= 0){
    //     hours += 24;
    // }
    printInt(hours);
    printf(":");
    int minutes = sys_time(MINUTES);
    if(minutes < 10)
        printf("0");
    printInt(minutes);
    printf(", ");
    printInt(sys_time(DAY_OF_THE_MONTH));
    printf("/");
    printInt(sys_time(MONTH));
    printf("/");
    printInt(sys_time(YEAR) + 2000);
    printf("\n");
}