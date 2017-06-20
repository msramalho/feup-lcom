#include <minix/syslib.h>
#include <minix/drivers.h>

#include "macros.h"
int hook_id = 0; //variable to deal with kernel interrupts

int timer_subscribe_int(void) {
	int mask = BIT(hook_id);
	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != OK) {
		printf("sys_irqsetpolicy failed!\n");
		return -1;
	}
	if (sys_irqenable(&hook_id) != OK) {
		printf("sys_irqenable failed!\n");
		return -1;
	}
	return mask;
}

int timer_unsubscribe_int() {
	if (sys_irqdisable(&hook_id) != OK) {
		printf("sys_irqdisable failed!\n");
		return -1;
	}
	if (sys_irqrmpolicy(&hook_id) != OK) {
		printf("sys_irqenable failed!\n");
		return -1;
	}
	return 0;
}

