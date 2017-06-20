#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <math.h>
#include "i8042.h"

int hook_id_ps2 = 3;


typedef enum {INIT, DRAW, COMP} state_t;
typedef enum {RDOW, RUP, MOVE, NOTHING} event_t;

int is_bit (unsigned char c, unsigned long pos) {
	return c >> pos & 0x01;
}

int two_to_decimal(unsigned char bin, int negative) {
    int sum = 0;
    int power = pow(2,7);
    int i = 0;
    if(negative == 1){
    	bin = ~bin;
    	bin++;
    }
    for(;i<7; i++){
    	sum += (bin >> i & 0x01)*power;
        power /= 2;
    }
    return negative?-sum:sum;
}

int ps2_subscribe_int(void){
	int mask = BIT(hook_id_ps2);
	if(sys_irqsetpolicy(PS2_IRQ, IRQ_EXCLUSIVE | IRQ_REENABLE, & hook_id_ps2) != OK){
		printf("sys_irqsetpolicy failed for mouse!\n");
		return -1;
	}
	if(sys_irqenable(& hook_id_ps2) != OK){
		printf("sys_irqenable failed for mouse!\n");
		return -1;
	}
	/*int r = write_command_to_mouse(ENABLE_STREAM_MODE);
	printf("RESULT of write_command_to_mouse(ENABLE_STREAM_MODE): %d\n", r);
	if(r != 0){
		printf("Failed to set stream mode!\n");
		return -1;
	}
	r = write_command_to_mouse(MOUSE_ENABLE);
	printf("RESULT of write_command_to_mouse(MOUSE_ENABLE): %d\n", r);
	if(r != 0){
		printf("Failed to enable mouse!\n");
		return -1;
	}*/
	if(write_cmd_attempts(ENABLE_STREAM_MODE, MOUSE_MAX_ATTEMPTS) != 0){
		printf("Failed to set stream mode!\n");
		return -1;
	}
	if(write_cmd_attempts(MOUSE_ENABLE, MOUSE_MAX_ATTEMPTS) != 0){
		printf("Failed to enable mouse!\n");
		return -1;
	}
	printf("Mouse subscribed successfully!\n");
	return mask;
}

int ps2_unsubscribe_int() {
	if(write_cmd_attempts(DISABLE_STREAM_MODE, MOUSE_MAX_ATTEMPTS) != 0){
		printf("Failed to disable mouse!\n");
		return -1;
	}
	if(sys_irqdisable(& hook_id_ps2) != OK){
		printf("sys_irqdisable failed for mouse!\n");
		return -1;
	}
	if(sys_irqrmpolicy(& hook_id_ps2) != OK){
		printf("sys_irqrmpolicy failed for mouse!\n");
		return -1;
	}
	return 0;
}

int write_byte_to_port(unsigned long p, unsigned long b){
	unsigned long mouse_status;
	//printf("reading status\n");
	if(sys_inb(STATUS_REGISTER, &mouse_status) != OK){
		return -1;
	}

	//printf("testing INPUT_BUFFER_FULL %X\n", mouse_status);
	if((mouse_status & INPUT_BUFFER_FULL) == INPUT_BUFFER_FULL){
		return -1;
	}

	//printf("sending sys_outb\n");
	if(sys_outb(p, b) != OK){
		//printf("byte not written\n");
		return -1;
	}else{
		//printf("byte written\n");
		return 0;
	}
}


unsigned long read_from_out_buf(){
	unsigned long mouse_status, data;

	if(sys_inb(STATUS_REGISTER, &mouse_status) != OK){
		return -1;
	}

	if((mouse_status & OUTPUT_BUFFER_FULL)!=OUTPUT_BUFFER_FULL){
		return -1;
	}

	if(sys_inb(OUTPUT_BUFFER, &data) != OK){
		return -1;
	}
	return data;
}

int write_command_to_mouse(unsigned long c){
	if(write_byte_to_port(STATUS_REGISTER, WRITE_BYTE_TO_MOUSE)!=0){//send the next command directly to the mouse
		return -3;
	}
	if(write_byte_to_port(COMMAND_REGISTER, c)!=0){
		return -4;
	}
	tickdelay(micros_to_ticks(DELAY_US));//wait a reasonable time for the response
	unsigned long r = read_from_out_buf();
	if(r == ACK){
		return 0;
	}else if(r == NACK){
		return -1;
	}else{
		return -2;
	}
	//return r == ACK?0:r == NACK?-1:-2;
}

int write_cmd_attempts(unsigned long c, unsigned int attempts){
	int i = 1;
	for(; i <= attempts; i++){
		if(write_command_to_mouse(c) == 0){
			return 0;
		}
	}
	return -1;
}



void print_packet(unsigned char b1, unsigned char b2, unsigned char b3){
	//B1=0x8 	B2=0x12 	B3=0x14 	LB=0		MB=0 	RB=0 	XOV=0 	YOV=0 	X=18 	Y=20
	printf("B1=0x%X B2=0x%X B3=0x%X LB=%d MB=%d RB=%d XOV=%d YOV=%d X=%d Y=%d \n", b1, b2, b3,
			(is_bit(b1,0)?1:0),//lb
			(is_bit(b1,2)?1:0),//mb
			(is_bit(b1,1)?1:0),//rb
			(is_bit(b1,6)?1:0),//xov
			(is_bit(b1,7)?1:0),//yov
			two_to_decimal(b2, is_bit(b1,4)),//x value
			two_to_decimal(b2, is_bit(b1,5))//y value
			);
	//printf("Decimal of 0x%X = %d\n", b2, two_to_decimal(b2, is_bit(b1,6)));
	//printf("Decimal of 0x%X = %d\n", b3, two_to_decimal(b3, is_bit(b1,5)));

}

unsigned long ps2_int_handler(unsigned char *packet, unsigned short *counter, unsigned short *current_byte){
	unsigned char b = read_from_out_buf();
	if(b != -1){
		if((*current_byte) == 3){
			(*current_byte)=1;
		}else{
			(*current_byte)++;
		}
		if((BIT(3) & b) == 0 && *current_byte == 1){//this is not the first byte
			(*current_byte) = 2;
			//printf("FIXED CURRENT %d - %X (PACKET %d/3)\n", (*counter), b, (*current_byte));
		}
		if(*current_byte == 1 && *counter >= 3){
			print_packet(packet[*counter - 3], packet[*counter - 2], packet[*counter - 1]);
			//int i = *counter - 3;
			/*for(;i < *counter; i++){
				printf("(%d, %X) - ", i, packet[i]);
			}*/
			//printf("\n--------------PACKET:\n");
		}
		//printf("READ BYTE %d - %X (PACKET %d/3)\n", (*counter), b, (*current_byte));
		packet[*counter] = b;
		(*counter)++;
		return b;
	}
	return -1;
}

unsigned long ps2_int_handler_no_memory(){
	unsigned char b = read_from_out_buf();
	if(b != -1){
		printf("READ BYTE %X \n", b);
		return b;
	}
	return -1;
}




/*bool check_pos_slope(event_t * evt) {
	static state_t st = INIT; // initial state; keep state
	switch (st) {
	case INIT:
		if (evt->type == RDOWN)
			state = DRAW;
		break;
	case DRAW:
		if (evt->type == MOVE) {
			return 1;
		} else if (evt->type == RUP)
			state = INIT;
		break;
	default:
		break;
	}
	return 0;
}*/

/*struct {
    int width;
    int height;
 } screen;*/

int ps2_gesture_handler(int length){
	static unsigned short current_byte = 0;
	static event_t last_evt = RUP;
	static unsigned char * packet;
	if (!packet){
		packet = (unsigned char *) malloc(sizeof(unsigned char) * 3);
		packet[0] = packet[1] = packet[2] = 0x00;
	}


	unsigned char b = read_from_out_buf();
	if(b != -1){
		if((current_byte) == 3){
			(current_byte)=1;
		}else{
			(current_byte)++;
		}
		if((BIT(3) & b) == 0 && current_byte == 1){//this is not the first byte
			(current_byte) = 2;
			//printf("FIXED CURRENT %d - %X (PACKET %d/3)\n", (*counter), b, (*current_byte));
		}
		//printf("READ BYTE %d - %X (PACKET %d/3)\n", (*counter), b, (*current_byte));

		printf("current_byte: %d\n", current_byte);
		packet[current_byte -1] = b;
		printf("current_byte: done\n");

		event_t evt = NOTHING;
		if(current_byte == 3){//full packet
			//STATE MACHINE
			printf("cDistance\n");

			printf("Distance: %s \n", sqrt(//distance is greater than the length
					pow(two_to_decimal(packet[1], is_bit(packet[0],4)),2)
					+
					pow(two_to_decimal(packet[2], is_bit(packet[0],5)),2))
				);
			if(last_evt == RUP && is_bit(packet[0],1)){//if previous is RUP and now right is down
				evt = RDOW;
				printf("RDOWN \n");
			}else if(last_evt == RDOW && !is_bit(packet[0],1)){//if previous is RDOW and now right is up
				evt = RUP;
				printf("RUP \n");
			}else if(two_to_decimal(packet[1], is_bit(packet[0],4)) > 0 &&//x value is positive AND
					two_to_decimal(packet[2], is_bit(packet[0],5)) > 0
					&& (sqrt(//distance is greater than the length
							pow(two_to_decimal(packet[1], is_bit(packet[0],4)),2)
							+
							pow(two_to_decimal(packet[2], is_bit(packet[0],5)),2))
						) > length){//y value is positive
				evt = MOVE;
				printf("MOVE \n");
			}else{
				evt = NOTHING;
				printf("NOTHING \n");
			}
		}
		printf("here\n");


		last_evt = evt;
		return evt == MOVE?1:0;
	}
	return NOTHING;
}















