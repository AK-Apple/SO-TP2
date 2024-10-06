#include "process.h"
#include "lib.h"
#include "IO.h"

// diccionario con nombres de funciones y funciones
// int64_t function(int argc, char* argv[]);
// Desventaja: todas las funciones deben tener argc y argv

// Alternativa: hacer un Switch

// TODO: realizar exit, strcmp y printf con %d

#define SIZE_DICTIONARY 64




// Funciones:
int64_t endless_loop(int argc, char* argv[]);
int64_t endless_loop_print(int argc, char* argv[]);


typedef int64_t (*FunctionPointer)(int argc, char* argv[]);

struct DictionaryEntry {
    char* name;
    FunctionPointer function;
};

struct DictionaryEntry dictionary[SIZE_DICTIONARY] = {
    {"endless_loop", endless_loop},
    {"endless_loop_print", endless_loop_print},
    // Add up to 64 entries
};

FunctionPointer findFunctionByName(const char* name) {
    for (int i = 0; i < SIZE_DICTIONARY; i++) {
        if (dictionary[i].name != NULL && k_strcmp(dictionary[i].name, name) == 0) {
            return dictionary[i].function;  // Return the function pointer if the name matches
        }
    }
    return NULL;  // Return NULL if the name is not found
}

uint64_t default_rip = 0;

// TODO: crear un initializer para no tener que estar haciendo comparaciones todo el tiempo
uint64_t get_rip() {
  if (!default_rip) {
    process_initializer(0, 0, 0);
  }
  return default_rip;
}


uint64_t process_initializer(char* name, int argc, char* argv[]) {
  if (!default_rip) {
    return 0;
  }

    FunctionPointer func = findFunctionByName(name);
    return func(argc, argv);
    // exit(return_value);
}

// Dummies
void bussy_wait(uint64_t n) {
  uint64_t i;
  for (i = 0; i < n; i++)
    ;
}

int64_t endless_loop(int argc, char* argv[]) {
  while (1)
    ;
}

int64_t endless_loop_print(int argc, char* argv[]) {
  if (argc != 1) return 1;
  

  uint64_t wait = k_atoi(argv[0]);

  int64_t pid = getpid();

  while (1) {
    // printf("%d ", pid);
    k_print_int_dec(pid);
    printf(" ");
    bussy_wait(wait);
  }
}