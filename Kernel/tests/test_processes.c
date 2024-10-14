#include "IO.h"
#include "lib.h"
#include "scheduling.h"
#include "test_util.h"
#include "time.h"

typedef struct P_rq
{
    int32_t pid;
    State state;
} p_rq;

void stack_frame() {
    char str[] = "perdido";
    char *argv1[] = {"arg1", "arg2", "arg3", "arg4", "arg5", "arg6", str, "arg7"};
    create_process("arg_print", sizeof(argv1) / sizeof(char*), argv1);
}

void javitest() {
    printf("\njavitest start____________________\n");
        char * argvaux[] = {"500"};
    stack_frame();
    printf("hola\n");
    // while(1);
    create_process("endless_loop_print", 1, argvaux);
    printf("Primer proceso creado\n");
    create_process("endless_loop_print", 1, argvaux);
    printf("Segundo proceso creado\n");
    create_process("endless_loop_print", 1, argvaux);
    printf("Tercer proceso creado\n");


    printf("\njavitest stop________________\n");
}

void prioritytest() 
{
    char * argvaux[] = {"500"};
    uint64_t pid_1 = create_process("endless_loop_print", 1, argvaux);
    uint64_t pid_2 = create_process("endless_loop_print", 1, argvaux);
    change_priority(pid_1, 4);
    // change_priority(pid_1, -4);
    while(1);
}

void waitpidtest() 
{
    char * argvaux[] = {"500"};
    uint64_t pid_1 = create_process("sleepy_process", 1, argvaux);
    uint64_t pid_2 = create_process("sleepy_process", 1, argvaux);
    uint64_t pid_3 = create_process("sleepy_process", 1, argvaux);
    // uint64_t pid4 = create_process("endless_loop", 1, argvaux);
    children_wait();
    printf("\nesto se imprime despues de que el todos despierten\n");
    while(1);
}

int64_t test_processes()
{
    waitpidtest();
    return;
    uint8_t rq;
    uint8_t alive = 0;
    uint8_t action;
    uint64_t max_processes;
    char *argvAux[] = {0};

    max_processes = MAX_PROCESS_BLOCKS - 1;

    p_rq p_rqs[max_processes];

    while (1)
    {

        // Create max_processes processes
        for (rq = 0; rq < max_processes; rq++)
        {
            p_rqs[rq].pid = create_process("endless_loop", 0, argvAux);

            if (p_rqs[rq].pid == -1)
            {
                printf("test_processes: ERROR creating process\n");
                return -1;
            }
            else
            {
                p_rqs[rq].state = RUNNING;
                alive++;
            }
        }
        // Randomly kills, blocks or unblocks processes until every one has been killed
        get_all_processes();
        while (alive > 0)
        {

            for (rq = 0; rq < max_processes; rq++)
            {
                action = GetUniform(100) % 2;

                switch (action)
                {
                case 0:
                    if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED)
                    {
                        if (kill_process(p_rqs[rq].pid) == -1)
                        {
                            printf("test_processes: ERROR killing process\n");
                            return -1;
                        }
                        p_rqs[rq].state = UNAVAILABLE;
                        alive--;
                    }
                    break;

                case 1:
                    if (p_rqs[rq].state == RUNNING)
                    {
                        if (block(p_rqs[rq].pid) == -1)
                        {
                            printf("test_processes: ERROR blocking process\n");
                            return -1;
                        }
                        p_rqs[rq].state = BLOCKED;
                    }
                    break;
                }
            }

            // Randomly unblocks processes
            for (rq = 0; rq < max_processes; rq++)
                if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2)
                {
                    if (unlock(p_rqs[rq].pid) == -1)
                    {
                        printf("test_processes: ERROR unblocking process\n");
                        return -1;
                    }
                    p_rqs[rq].state = RUNNING;
                }
        }
       
    }
}