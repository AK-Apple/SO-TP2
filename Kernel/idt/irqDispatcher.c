// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "time.h"
#include <stdint.h>
#include "keyboard.h"

void irqDispatcher(uint64_t irq, uint64_t rsp) {
	switch (irq) {
		case 0:
			timer_handler();
			break;
        case 1:
		 	keyboard_handler(rsp);
		 	break;
	}
	return;
}
