#include <minix/syslib.h>
#include <minix/drivers.h>
#include "i8254.h"

int hook_id = 0;//variable to deal with kernel interrupts
unsigned int count_freq;

int timer_display_conf(unsigned char conf);

int is_bit (unsigned char c, unsigned long pos) {
	return c >> pos & 0x01;
}

int timer_set_square(unsigned long timer, unsigned long freq) {
	printf("SET SQUARE START\n");
	unsigned long div = TIMER_FREQ/freq;
	unsigned long control_word = timer<<6;
	unsigned long lsb = div, msb = (div >>8);
	unsigned char conf;

	timer_get_conf(timer, &conf);
	timer_display_conf(conf);

	if(timer > 2){
		printf("The specified timer (%u) does not exist\n", timer);
		return -1;
	}
	printf("check timer %u\n", timer);

	//send control word
	//TIMER_LSB_MSB - xx110000, counter initialization: LSB followed by MSB
	//TIMER_SQR_WAVE - xx11x110, MODE 3 (square wave)
	//TIMER_BIN - xx11x110 (optional since BIN is the default)
	control_word |= TIMER_LSB_MSB |TIMER_SQR_WAVE;

	if(sys_outb(TIMER_CTRL, control_word)!=OK){
		printf("Failed sys_outb control_word");
		return -1;
	}
	//send lsb + msb
	printf("sys_outb TIMER_CTRL good");
	if(sys_outb(TIMER_0 + timer, lsb)!=OK){
		printf("Failed sys_outb");
		return -1;
	}
	printf("lsb good");
	if(sys_outb(TIMER_0 + timer, msb)!=OK){
		printf("Failed sys_outb");
		return -1;
	}
	printf("msb good");

	printf("SET SQUARE DONE\n");
	return 0;
}

int timer_subscribe_int(void) {
	int mask = BIT(hook_id);
	//TIMER0 IRQ, inform the GIH that it can give the EOI command, hook
	if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, & hook_id) != OK){
		printf("sys_irqsetpolicy failed!\n");
		return -1;
	}
	if(sys_irqenable(& hook_id) != OK){
		printf("sys_irqenable failed!\n");
		return -1;
	}
	return mask;
}

int timer_unsubscribe_int() {
	if(sys_irqdisable(& hook_id) != OK){
		printf("sys_irqdisable failed!\n");
		return -1;
	}
	if(sys_irqrmpolicy(& hook_id) != OK){
		printf("sys_irqenable failed!\n");
		return -1;
	}
	return 0;
}

void timer_int_handler() {
	if((++count_freq) % 60 == 0){
		printf("Time since start: %u seconds\n", (count_freq / 60));
	}
}

int timer_get_conf(unsigned long timer, unsigned char *st) {
	printf("timer_get_conf start\n");
	unsigned long read_back_bits = TIMER_RB_CMD| TIMER_RB_SEL(timer) | TIMER_RB_COUNT_;// | TIMER_RB_STATUS_;//
	unsigned long result_read_back;

	//Requesting the readback command
	//bits 6 and 7 to 1 11000000
	//set get status to 1 1101
	//set the counter for the current timer

	if(timer > 2){
		printf("The specified timer (%u) does not exist\n", timer);
		return -1;
	}
	if(sys_outb(TIMER_CTRL, read_back_bits)!=OK){
		printf("Failed sys_outb\n");
		return 1;
	}else{
		printf("ALL GOOD sys_outb\n");
	}
	//if(sys_outb(TIMER_CTRL, read_back_bits)!=OK)printf("Failed sys_outb\n");return 1;

	if(sys_inb(TIMER_0 + timer, &result_read_back)!=OK){
		printf("Failed sys_inb\n");return 1;
	}else{
		printf("ALL GOOD sys_inb\n");
	}
	printf("Result of st: %x\n", result_read_back);

	*st = (unsigned char) result_read_back;
	printf("timer_get_conf end\n");
	return 0;
}

int timer_display_conf(unsigned char conf) {
	printf("timer_display_conf start\n");
	printf("Out line value: %x\n", conf>>7);

	if(is_bit(conf, 7))printf("square wave value is 1\n");
	else printf("square wave value is 0\n");
	if(is_bit(conf, 6))printf("null count\n");
	else printf("count available for reading\n");
	//type of access
	if(is_bit(conf,5) && is_bit(conf, 4))printf("LSB and MSB\n");
	else if(is_bit(conf,4))printf("LSB\n");
	else printf("MSB\n");
	//programmed mode
	if(is_bit(conf,1)&&is_bit(conf, 2))printf("Square Wave mode (3)\n");
	else if (!is_bit(conf,1)&&!is_bit(conf, 2)&&!is_bit(conf, 3))printf("mode 0\n");
	else if (is_bit(conf,1)&&!is_bit(conf, 2)&&!is_bit(conf, 3))printf("mode 1\n");
	else if (!is_bit(conf,1)&&is_bit(conf, 2))printf("mode 2\n");
	else if (!is_bit(conf,1)&&!is_bit(conf, 2)&&is_bit(conf, 3))printf("mode 4\n");
	else if (is_bit(conf,1)&&!is_bit(conf, 2)&&is_bit(conf, 3))printf("mode 5\n");
	//add other modes if necessary

	if(is_bit(conf, 0))printf("BCD\n");
	else printf("Binary\n");

	printf("timer_display_conf end\n");
	return 0;
}

int timer_test_square(unsigned long freq) {
	timer_set_square(0, freq);
	return 1;
}

int timer_test_int(unsigned long time) {
	int ipc_status, irq_set, r;
	message msg;
	count_freq = 0;

	if((irq_set = timer_subscribe_int()) == -1){
		printf("unable to subscribe\n");
		return 1;
	}

	while(count_freq/60 < time) {
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set) {
					timer_int_handler();
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}
	if (timer_unsubscribe_int() != 0){
		printf("unable to unsubscribe\n");
		return 1;
	}
	printf("timer interrupt is done\n");
	return 0;
}

int timer_test_config(unsigned long timer) {
	unsigned char st;
	timer_get_conf(timer, &st);
	timer_display_conf(st);
	return 1;
}


















