#include <minix/syslib.h>
#include "i8042.h"

unsigned short *leds_array;
unsigned short leds_state = 0x00;
unsigned short leds_enabled[3]={0};//scroll_lock = 0, num_lock = 0, caps_lock = 0;//assuming all leds are disabled at the beginning of the program

void change_led(unsigned long count){
	int current_led_index = (count/60) - 1;
	printf("-------------------------------\nLed %d\n", leds_array[current_led_index]);

	if(write_to_kbc(KBD_LEDS_COMMAND) == 0){//the command was written
		if(write_to_kbc(LED(leds_array[current_led_index])) == 0){//if the arguments are written properly
			//change leds saved state
			if(leds_enabled[leds_array[current_led_index]] == 0){
				leds_enabled[leds_array[current_led_index]] = 1;
				printf("LED %d ENABLED\n");
			}else{
				leds_enabled[leds_array[current_led_index]] = 0;
				printf("LED %d DISABLED\n");
			}
		}
	}

}


int kbd_test_scan(unsigned short ass) {
	kbd_test_int(ass);
	//timer_test_int(10, print_message, 60);//duration: 10 seconds, callback every 60 miliseconds = 1 second
}
int kbd_test_leds(unsigned short n, unsigned short *leds) {
	leds_array = leds;
	timer_test_int(n, change_led, 60);//duration: n seconds, callback every 60 miliseconds = 1 second, call change_led function
	return 0;
}
int kbd_test_timed_scan(unsigned short n) {
	int ipc_status, irq_set_timer, irq_set_kbd, r, proceed = 1;
	message msg;
	unsigned long count_freq = 0;

	if((irq_set_timer = timer_subscribe_int()) == -1){
		printf("unable to subscribe timer\n");
		return 1;
	}
	if((irq_set_kbd = kbd_subscribe_int()) == -1){
		printf("unable to subscribe kbd\n");
		return 1;
	}

	while(count_freq/60 < n && proceed) {
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set_timer) {
					count_freq++;
				}else if(msg.NOTIFY_ARG & irq_set_kbd){
					count_freq = 0;
					if(kbd_int_handler() == ESC_KEY){
						proceed = 0;
					}//print scancodes
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		}
	}
	if (timer_unsubscribe_int() != 0){
		printf("unable to unsubscribe timer\n");
		return 1;
	}
	if (kbd_unsubscribe_int() != 0){
		printf("unable to unsubscribe kbd\n");
		return 1;
	}
	if(proceed == 1){
		printf("Timed out of kbd_test_timed_scan \n");
	}else{
		printf("Pressed the ESC Key \n");
	}
	return 0;
}














