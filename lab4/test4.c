#include <minix/syslib.h>
#include "i8042.h"

int test_packet(unsigned short cnt){
	unsigned char *packet;//to store the packet bytes
	unsigned short counter = 0, current_byte = 0;//to keep track of byte number

	packet = (unsigned char *) malloc(sizeof(unsigned char) * (cnt * 3 + 3));

	int ipc_status, irq_set_ps2, r;
	message msg;

	if((irq_set_ps2 = ps2_subscribe_int()) == -1){
		printf("unable to subscribe ps/2\n");
		return -1;
	}

	while(counter < cnt) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0){
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if(msg.NOTIFY_ARG & irq_set_ps2){
					ps2_int_handler(packet, &counter, &current_byte);
				}
				break;
			default:
				break;
			}
		}
	}

	if (ps2_unsubscribe_int() != 0){
		printf("unable to unsubscribe ps/2\n");
		return 1;
	}

	return 0;
}	
	
int test_async(unsigned short idle_time) {
	int ipc_status, irq_set_timer, irq_set_ps2, r;
	message msg;
	unsigned long count_freq = 0;

	if((irq_set_timer = timer_subscribe_int()) == -1){
		printf("unable to subscribe timer\n");
		return -1;
	}
	if((irq_set_ps2 = ps2_subscribe_int()) == -1){
		printf("unable to subscribe ps/2\n");
		return -1;
	}

	while(count_freq/60 < idle_time) {
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set_timer) {
					count_freq++;
				}else if(msg.NOTIFY_ARG & irq_set_ps2){
					count_freq = 0;
					ps2_int_handler_no_memory();
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		}
	}
	if (timer_unsubscribe_int() != 0){
		printf("unable to unsubscribe timer\n");
		return -1;
	}
	if (ps2_unsubscribe_int() != 0){
		printf("unable to unsubscribe ps/2\n");
		return -1;
	}
	printf("DONE");
	return 0;
}
	
int test_config(void) {
	if(ps2_subscribe_int() == -1){
		printf("unable to subscribe ps/2\n");
		return -1;
	}
	//disable stream mode
	if(write_cmd_attempts(DISABLE_STREAM_MODE, MOUSE_MAX_ATTEMPTS) != 0){
		printf("Failed to set stream mode!\n");
		return -1;
	}


	//request status
	if(write_cmd_attempts(STATUS_REQUEST, MOUSE_MAX_ATTEMPTS) != 0){
		printf("Failed to complete the status request!\n");
		return -1;
	}

	//read the three bytes
	unsigned long
			conf1 = read_from_out_buf(),
			conf2 = read_from_out_buf(),
			conf3 = read_from_out_buf();
	printf("\n--------------------------------Mouse configuration:\n");
	printf("Configuration bytes: 0x%X 0x%X 0x%X \n", conf1, conf2, conf3);
	printf("Remote: %s \n", (is_bit(conf1, 6)?"On":"Off"));
	printf("Enable: %s \n", (is_bit(conf1, 5)?"data report on":"data report off"));
	printf("Scaling: %s \n", (is_bit(conf1, 4)?"2:1":"1:1"));
	printf("LB: %d, MB: %d, RB: %d \n",
			is_bit(conf1, 2),//lb
			is_bit(conf1, 1),//mb
			is_bit(conf1, 0)//rb
			);
	printf("Resolution: %d \n", conf2);
	printf("Sample rate(10 - 200): %d \n", conf3);

	if (ps2_unsubscribe_int() != 0){
		printf("unable to unsubscribe ps/2\n");
		return -1;
	}
	return 0;
}	

int test_gesture(short length) {
		//unsigned char *packet;//to store the packet bytes
		//unsigned short counter = 0, current_byte = 0;//to keep track of byte number

		//packet = (unsigned char *) malloc(sizeof(unsigned char) * (cnt * 3 + 3));

		int ipc_status, irq_set_ps2, r, proceed = 1;
		message msg;

		if((irq_set_ps2 = ps2_subscribe_int()) == -1){
			printf("unable to subscribe ps/2\n");
			return -1;
		}

		while(proceed) {
			if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0){
				printf("driver_receive failed with: %d", r);
				continue;
			}
			if (is_ipc_notify(ipc_status)) {
				switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE:
					if(msg.NOTIFY_ARG & irq_set_ps2){
						if(ps2_gesture_handler(length)){
							proceed = 0;
						}
					}
					break;
				default:
					break;
				}
			}
		}

		if (ps2_unsubscribe_int() != 0){
			printf("unable to unsubscribe ps/2\n");
			return 1;
		}

		return 0;
}

















