#ifndef SLEEP_MANAGER_H
#define SLEEP_MANAGER_H

#include <stdint.h>
#include "shared.h"

void ticks_sleep(uint64_t ticks);
void seconds_sleep(uint64_t seconds);
void wake_available();
void wake_forced(pid_t pid);

#endif