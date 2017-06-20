#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include "i8042.h"

int hook_id_kbd = 1;
int double_byte_counter = 0;
unsigned char scan;//variable used in assembly code

int is_bit (unsigned char c, unsigned long pos) {
	return c >> pos & 0x01;
}

int kbd_subscribe_int(void) {
	int mask = BIT(hook_id_kbd);
	if(sys_irqsetpolicy(KBD_IRQ, IRQ_EXCLUSIVE | IRQ_REENABLE, & hook_id_kbd) != OK){
		printf("sys_irqsetpolicy failed for keyboard!\n");
		return -1;
	}
	if(sys_irqenable(& hook_id_kbd) != OK){
		printf("sys_irqenable failed for keyboard!\n");
		return -1;
	}
	return mask;
}

int kbd_unsubscribe_int() {
	if(sys_irqdisable(& hook_id_kbd) != OK){
		printf("sys_irqdisable failed for keyboard!\n");
		return -1;
	}
	if(sys_irqrmpolicy(& hook_id_kbd) != OK){
		printf("sys_irqenable failed for keyboard!\n");
		return -1;
	}
	return 0;
}

void print_scancode(unsigned long scancode){
	if (scancode != KBD_TWO_BYTES_CODE){
		if (is_bit (scancode, 7)) {
			printf("Breakcode: ");
		}else{
			printf("Makecode: ");
		}
		if (double_byte_counter == 0) {
			printf("0x%X\n", scancode);
		}else{
			printf("0xE0 - 0x%X\n", scancode);
			double_byte_counter = 0;
		}
	}
	else double_byte_counter = 1;

}

unsigned long kbd_int_handler(){
	unsigned long kbd_status, scancode;

	if(sys_inb(STATUS_REGISTER, &kbd_status) != OK){
		printf("Unable to read kbd status\n");
		return -1;
	}

	if(!(kbd_status & OUTPUT_BUFFER_FULL)){
		printf("The output buffer is not full\n");
		return -1;
	}

	if(sys_inb(OUTPUT_BUFFER, &scancode)!=OK){
		printf("Unable to read the scancode\n");
		return -1;
	}

	print_scancode(scancode);
	return scancode;
}

int kbd_test_int(unsigned long ass) {
	unsigned long kbd_status;
	int ipc_status, irq_set, r;
	int proceed = 1;
	message msg;

	if((irq_set = kbd_subscribe_int()) == -1){
		printf("unable to subscribe\n");
		return 1;
	}

	while(proceed) {
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set) {
					if (ass == 0){
						if(kbd_int_handler() == ESC_KEY){
							proceed = 0;
						}
					} else if(ass == 1){
						proceed = 0;
						/*
						printf("1\n");

						printf("Starting assembly code\n");
						if(sys_inb(STATUS_REGISTER, &kbd_status) != OK){
							printf("Unable to read kbd status\n");
							continue;
						}
						printf("2\n");

						if(!(kbd_status & OUTPUT_BUFFER_FULL)){
							printf("The output buffer is not full\n");
							continue;
						}
						printf("3\n");
						//scan = ESC_KEY;
						scan = kbd_int_handler_ass();
						printf("Success\n");
						print_scancode(scan);
						if(scan == ESC_KEY){
							proceed = 0;
						}*/
					}
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}
	if (kbd_unsubscribe_int() != 0){
		printf("unable to unsubscribe\n");
		return 1;
	}
	printf("kbd interrupt is done\n");
	return 0;
}

int write_to_kbc(unsigned long cmd){
	unsigned long kbd_status, kbd_return;
	int count_fails = 0;
	printf("Writing 0X%X  -  ", cmd);
	while(count_fails < MAX_KBC_ATTEMPTS) {
		kbd_return = 0x00;
		if(sys_inb(STATUS_REGISTER, &kbd_status) != OK){
			count_fails++;
			continue;
		}

		/*loop while 8042 input buffer is not empty*/
		if((kbd_status & INPUT_BUFFER_FULL) == 0){
			if(sys_outb(COMMAND_REGISTER, cmd) != OK){
				count_fails++;
				continue;
			}
			read_from_kbc(&kbd_return);
			if(kbd_return == ACK){
				printf("Acknowledged\n");
				return 0;
			}else if (kbd_return == RESEND){
				printf("RESEND\n");
				count_fails++;
			}else if(kbd_return == ERROR){
				printf("ERROR\n");
				return -1;
			}else{
				printf("KBC_RETURN '0x%X' unidentified\n");
				count_fails++;
			}
		}
		tickdelay(micros_to_ticks(DELAY_US));//wait a reasonable time
		count_fails++;
	}
	return -1;
}

int read_from_kbc(unsigned long *data){
	unsigned long kbd_status;
	int count_fails = 0;
	while(count_fails < MAX_KBC_ATTEMPTS) {
		if(sys_inb(STATUS_REGISTER, &kbd_status) != OK){
			count_fails++;
			continue;
			printf("Unable to read kbd status before writing command\n");
		}
		/*loop while 8042 output buffer is empty*/
		if(!(kbd_status & OUTPUT_BUFFER_FULL)){
			if(sys_inb(OUTPUT_BUFFER, data) != OK){//read from the output
				printf("Unable to read the data\n");
				count_fails++;
				continue;
			}

			if ((kbd_status & (ERROR_PARITY | ERROR_TIMEOUT)) == 0){
				return 0;
			}else{
				return -1;
			}
		}
		tickdelay(micros_to_ticks(DELAY_US));//wait a reasonable time
		count_fails++;
	}
	return -1;
}













