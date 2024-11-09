#ifndef SLEEP_MANAGER_H
#define SLEEP_MANAGER_H

#include <stdint.h>
#include "shared.h"

void ticks_sleep_2(uint64_t ticks);
void wake_available();
void wake_forced(pid_t pid);

#endif