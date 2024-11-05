#ifndef TPE_ARQUI_EXCEPTIONS_H
#define TPE_ARQUI_EXCEPTIONS_H

#include <stdint.h>
#include "process.h"

void exceptionDispatcher(int exception, StackedRegisters* stack);

#endif //TPE_ARQUI_EXCEPTIONS_H
