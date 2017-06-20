#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <math.h>
#include "macros.h"
#include "mouse.h"
#include "utils.h"
#include "io.h"

int hook_id_ps2 = 3;

mouse_t *new_mouse(unsigned int width, unsigned int height,
		unsigned char background, unsigned char font_color) {
	mouse_t *m = (mouse_t *) malloc(sizeof(mouse_t));
	if (m == NULL)
		return NULL;
	m->bytes[0] = 0;
	m->bytes[1] = 0;
	m->bytes[2] = 0;
	m->x = 0;
	m->y = 0;
	m->l_button = 0;
	m->m_button = 0;
	m->r_button = 0;
	m->counter = 0;
	m->frame = new_frame(width, height, background, font_color);
	return m;
}

void delete_mouse(mouse_t *m) {
	delete_frame(m->frame);
	free(m);
}

int mouse_subscribe_int(void) {
	int mask = BIT(hook_id_ps2);
	if (sys_irqsetpolicy(PS2_IRQ, IRQ_EXCLUSIVE | IRQ_REENABLE, &hook_id_ps2)
			!= OK) {
		printf("sys_irqsetpolicy failed for mouse!\n");
		return -1;
	}
	if (sys_irqenable(&hook_id_ps2) != OK) {
		printf("sys_irqenable failed for mouse!\n");
		return -1;
	}
	if (write_cmd_attempts(ENABLE_STREAM_MODE, MOUSE_MAX_ATTEMPTS) != 0) {
		printf("Failed to set stream mode!\n");
		return -1;
	}
	if (write_cmd_attempts(MOUSE_ENABLE, MOUSE_MAX_ATTEMPTS) != 0) {
		printf("Failed to enable mouse!\n");
		return -1;
	}
	return mask;
}

int mouse_unsubscribe_int() {
	if (write_cmd_attempts(DISABLE_STREAM_MODE, MOUSE_MAX_ATTEMPTS) != 0) {
		printf("Failed to disable mouse!\n");
		return -1;
	}
	if (sys_irqdisable(&hook_id_ps2) != OK) {
		printf("sys_irqdisable failed for mouse!\n");
		return -1;
	}
	if (sys_irqrmpolicy(&hook_id_ps2) != OK) {
		printf("sys_irqrmpolicy failed for mouse!\n");
		return -1;
	}
	return 0;
}

int ps2_int_handler(mouse_t *m) {
	unsigned char b = mouse_read();
	if (b != -1) {
		if ((m->counter) == 3) {
			(m->counter) = 1;
		} else {
			(m->counter)++;
		}
		if ((BIT(3) & b) == 0 && m->counter == 1) { //this is not the first byte
			(m->counter) = 2;
		}
		m->bytes[(m->counter) - 1] = b;
		if ((m->counter) == 3) {
			fill_frame(m->frame, TRANSPARENT);
			get_mouse_info(m);
			draw_mouse(m);
			return 1;
		}
		return 0;
	}
	return 0;
}

void draw_mouse(mouse_t * m) {
	int x = (m->x), y = (m->y);
	create_sprite(get_mouse_xpm(), x, y, m->frame);
}

unsigned long mouse_read() {
	unsigned long mouse_status, data;
	if (sys_inb(STATUS_REGISTER, &mouse_status) != OK) {
		printf("sys_inb failed\n");
		return -1;
	}
	if ((mouse_status & OUTPUT_BUFFER_FULL) != OUTPUT_BUFFER_FULL) {
		printf("OUTPUT_BUFFER_FULL failed\n");
		return -1;
	}
	if (sys_inb(OUTPUT_BUFFER, &data) != OK) {
		printf("sys_inb(OUTPUT_BUFFER_FULL failed\n");
		return -1;
	}
	return data;
}

char **get_mouse_xpm() {
	return mouse_xpm;
}

int two_to_decimal(unsigned char bin, int negative) {
	int sum = 0;
	int power = pow(2, 7);
	int i = 0;
	if (negative == 1) {
		bin = ~bin;
		bin++;
	}
	for (; i < 7; i++) {
		sum += (bin >> i & 0x01) * power;
		power /= 2;
	}
	return negative ? -sum : sum;
}

static void get_mouse_info(mouse_t *m) {
	if (m->bytes[0] & MOUSE_LB) {
		m->l_button = 1;
	} else
		m->l_button = 0;

	if (m->bytes[0] & MOUSE_MB) {
		m->m_button = 1;
	} else
		m->m_button = 0;

	if (m->bytes[0] & MOUSE_RB) {
		m->r_button = 1;
	} else
		m->r_button = 0;

	int delta;
	delta = two_to_decimal(m->bytes[1], is_bit(m->bytes[0], MOUSE_X_SIGNAL))
			/ 12;

	if ((m->x + delta) < 0) {
		m->x = 0;
	} else if ((m->x + delta) > m->frame->width) {
		m->x = m->frame->width;
	} else
		m->x += delta;

	delta = two_to_decimal(m->bytes[2], is_bit(m->bytes[0], MOUSE_Y_SIGNAL))
			/ 12;

	if ((m->y - delta) < 0) {
		m->y = 0;
	} else if ((m->y - delta) > m->frame->height) {
		m->y = m->frame->height;
	} else
		m->y -= delta;
}

void mouse_print_packet(mouse_t *m) {
	printf("B1=0x%X B2=0x%X B3=0x%X LB=%d MB=%d RB=%d X=%d Y=%d \n",
			m->bytes[0], m->bytes[1], m->bytes[2], m->l_button, //lb
			m->m_button, //mb
			m->r_button, //rb
			m->x, //x value
			m->y //y value
			);
}

int write_command_to_mouse(unsigned long c) {
	if (write_byte_to_port(STATUS_REGISTER, WRITE_BYTE_TO_MOUSE) != 0) { //send the next command directly to the mouse
		return -3;
	}
	if (write_byte_to_port(COMMAND_REGISTER, c) != 0) {
		return -4;
	}
	unsigned long r = mouse_read();
	if (r == ACK) {
		return 0;
	} else if (r == NACK) {
		return -1;
	} else {
		return -2;
	}
}

int write_cmd_attempts(unsigned long c, unsigned int attempts) {
	int i = 1;
	for (; i <= attempts; i++) {
		if (write_command_to_mouse(c) == 0) {
			return 0;
		}
	}
	return -1;
}

int write_byte_to_port(unsigned long p, unsigned long b) {
	unsigned long mouse_status;
	if (sys_inb(STATUS_REGISTER, &mouse_status) != OK) {
		return -1;
	}
	if ((mouse_status & INPUT_BUFFER_FULL) == INPUT_BUFFER_FULL) {
		return -1;
	}
	if (sys_outb(p, b) != OK) {
		return -1;
	} else {
		return 0;
	}
}

int get_mouse_line(mouse_t *m) {
	unsigned int n;
	if (m->y < 6 || m->y > (m->frame->height - 7)) {
		n = 0;
	} else {
		n = (m->y - 6) / 14 + 1;
	}
	return n - 1;
}

int get_mouse_column(mouse_t *m) {
	unsigned int n;
	if (m->x < 8 || m->x > (m->frame->width - 9)) {
		n = 0;
	} else {
		n = (m->x - 8) / 16 + 1;
	}
	return n - 1;
}

