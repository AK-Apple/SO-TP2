#include "process.h"
#include "lib.h"
#include "IO.h"
#include "interrupts.h"
#include "scheduling.h"

// diccionario con nombres de funciones y funciones
// int64_t function(int argc, char* argv[]);
// Desventaja: todas las funciones deben tener argc y argv

// Alternativa: hacer un Switch

// TODO: realizar exit, strcmp y printf con %d

#define SIZE_DICTIONARY 64

// Funciones:
int64_t endless_loop(int argc, char *argv[]);
int64_t endless_loop_print(int argc, char *argv[]);
int64_t arg_print(int argc, char *argv[]) {
  int j = 0;
  while( j != 10) 
  {
  printf("arg_print ");
  k_print_int_dec(argc);
  printf("[ ");
j++;
  for(int i = 0; i < argc; i++) {
    printf(" '");
    printf(argv[i]);
    printf("', ");
  }
  printf("]\n");
  yield();
  }
  while(j != 400) {
    if(j==30) kill_process(3);
    get_all_processes();
    if(j==300) {
      unlock(4);
    }
    if(j ==100) {

      block(4);
      block(2);
    }
    j++;
    yield();
  }
  // exit(0); 
}

typedef int64_t (*FunctionPointer)(int argc, char *argv[]);

struct DictionaryEntry
{
  char *name;
  FunctionPointer function;
};

struct DictionaryEntry dictionary[SIZE_DICTIONARY] = {
    {"endless_loop", endless_loop},
    {"endless_loop_print", endless_loop_print},
    {"arg_print", arg_print}
    // Add up to 64 entries
};

FunctionPointer findFunctionByName(const char *name)
{
  for (int i = 0; i < SIZE_DICTIONARY; i++)
  {
    if (dictionary[i].name != NULL && k_strcmp(dictionary[i].name, name) == 0)
    {
      return dictionary[i].function; // Return the function pointer if the name matches
    }
  }
  return NULL; // Return NULL if the name is not found
}


uint64_t process_initializer(char *name, int argc, char *argv[])
{
  printf(name);
  FunctionPointer func = findFunctionByName(name);
  uint64_t exit_code = func(argc, argv);
  exit(exit_code);
}