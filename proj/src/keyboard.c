#include "keyboard.h"
#include "macros.h"

#include <minix/drivers.h>

int hook_id_kbd = 1;

//-------------------------------------Key struct

static const unsigned int KEYS_SIZE = 47;
static const key keys[] = { { .hex = 0x1E, .lower = 'a', .upper = 'A', .altgr =
		'\0' }, { .hex = 0x30, .lower = 'b', .upper = 'B', .altgr = '\0' }, {
		.hex = 0x2E, .lower = 'c', .upper = 'C', .altgr = '\0' }, { .hex = 0x20,
		.lower = 'd', .upper = 'D', .altgr = '\0' }, { .hex = 0x12,
		.lower = 'e', .upper = 'E', .altgr = '\0' }, { .hex = 0x21,
		.lower = 'f', .upper = 'F', .altgr = '\0' }, { .hex = 0x22,
		.lower = 'g', .upper = 'G', .altgr = '\0' }, { .hex = 0x23,
		.lower = 'h', .upper = 'H', .altgr = '\0' }, { .hex = 0x17,
		.lower = 'i', .upper = 'I', .altgr = '\0' }, { .hex = 0x24,
		.lower = 'j', .upper = 'J', .altgr = '\0' }, { .hex = 0x25,
		.lower = 'k', .upper = 'K', .altgr = '\0' }, { .hex = 0x26,
		.lower = 'l', .upper = 'L', .altgr = '\0' }, { .hex = 0x27,
		.lower = 'c', .upper = 'C', .altgr = '\0' }, { .hex = 0x32,
		.lower = 'm', .upper = 'M', .altgr = '\0' }, { .hex = 0x31,
		.lower = 'n', .upper = 'N', .altgr = '\0' }, { .hex = 0x18,
		.lower = 'o', .upper = 'O', .altgr = '\0' }, { .hex = 0x19,
		.lower = 'p', .upper = 'P', .altgr = '\0' }, { .hex = 0x10,
		.lower = 'q', .upper = 'Q', .altgr = '\0' }, { .hex = 0x13,
		.lower = 'r', .upper = 'R', .altgr = '\0' }, { .hex = 0x1F,
		.lower = 's', .upper = 'S', .altgr = '\0' }, { .hex = 0x14,
		.lower = 't', .upper = 'T', .altgr = '\0' }, { .hex = 0x16,
		.lower = 'u', .upper = 'U', .altgr = '\0' }, { .hex = 0x2F,
		.lower = 'v', .upper = 'V', .altgr = '\0' }, { .hex = 0x11,
		.lower = 'w', .upper = 'W', .altgr = '\0' }, { .hex = 0x2D,
		.lower = 'x', .upper = 'X', .altgr = '\0' }, { .hex = 0x15,
		.lower = 'y', .upper = 'Y', .altgr = '\0' }, { .hex = 0x2C,
		.lower = 'z', .upper = 'Z', .altgr = '\0' }, { .hex = 0x0B,
		.lower = '0', .upper = '=', .altgr = '}' }, { .hex = 0x02, .lower = '1',
		.upper = '!', .altgr = '\0' }, { .hex = 0x03, .lower = '2',
		.upper = '"', .altgr = '@' }, { .hex = 0x04, .lower = '3', .upper = '#',
		.altgr = '\0' }, { .hex = 0x05, .lower = '4', .upper = '$', .altgr =
		'\0' }, { .hex = 0x06, .lower = '5', .upper = '%', .altgr = '\0' }, {
		.hex = 0x07, .lower = '6', .upper = '&', .altgr = '\0' }, { .hex = 0x08,
		.lower = '7', .upper = '/', .altgr = '{' }, { .hex = 0x09, .lower = '8',
		.upper = '(', .altgr = '[' }, { .hex = 0x0A, .lower = '9', .upper = ')',
		.altgr = ']' },
		{ .hex = 0x1A, .lower = '+', .upper = '*', .altgr = '\0' }, { .hex =
				0x35, .lower = '-', .upper = '_', .altgr = '\0' }, {
				.hex = 0x0C, .lower = '\'', .upper = '?', .altgr = '\0' }, {
				.hex = 0x34, .lower = '.', .upper = ':', .altgr = '\0' }, {
				.hex = 0x33, .lower = ',', .upper = ';', .altgr = '\0' }, {
				.hex = 0x29, .lower = '\\', .upper = '|', .altgr = '\0' }, {
				.hex = 0x2B, .lower = '~', .upper = '^', .altgr = '\0' }, {
				.hex = 0x39, .lower = ' ', .upper = ' ', .altgr = '\0' }, {
				.hex = 0x1B, .lower = '`', .upper = '\'', .altgr = '\0' }, {
				.hex = 0x56, .lower = '<', .upper = '>', .altgr = '\0' } };

//-------------------------------------Special Key struct
/** make and break codes of special keys*/
static special_key_t MASK = { .special_mask = 0x00 };
static int COUNT_SPECIAL = 7;
static const code_t special[] = { { .byte1 = 0x1D, .byte2 = 0x9D }, //CTRL
		{ .byte1 = 0x38, .byte2 = 0xB8 }, //ALT
		{ .byte1 = 0X2A, .byte2 = 0xAA }, //SHIFT1
		{ .byte1 = 0X36, .byte2 = 0xB6 }, //SHIFT2
		{ .byte1 = 0x38, .byte2 = 0xB8 }, //ALTGR
		{ .byte1 = 0x3A, .byte2 = 0xBA }, //CAPS LOCK
		{ .byte1 = 0x45, .byte2 = 0xC5 } }; //NUM LOCK

special_key_t * new_special_key() {
	special_key_t * s = malloc(sizeof(special_key_t));
	return s;
}

void update_special_key(special_key_t * s, unsigned long scancode,
		int double_key) {
	int i;
	for (i = 0; i < COUNT_SPECIAL - 3; i++) { //-3 because of ALTGR, caps lock and num lock which have a persistent behaviour
		if (!is_bit(s->special_mask, i) && scancode == special[i].byte1) { //makecode
			set_bit(&(s->special_mask), i, 1);
		} else if (is_bit(s->special_mask, i) && scancode == special[i].byte2) { //breakcode
			set_bit(&(s->special_mask), i, 0);
		}
	}
	//ALTGR requires 0xE0
	if (double_key) {
		if (!is_bit(s->special_mask, 4) && scancode == special[4].byte1) { //makecode
			set_bit(&(s->special_mask), 4, 1);
		} else if (is_bit(s->special_mask, 4) && scancode == special[4].byte2) { //breakcode
			set_bit(&(s->special_mask), 4, 0);
		}
	}
	//caps lock: toggle behaviour
	if (is_bit(s->special_mask, 5) && scancode == special[5].byte1) { //makecode
		set_bit(&(s->special_mask), 5, 0);
	} else if (!is_bit(s->special_mask, 5) && scancode == special[5].byte1) { //makecode
		set_bit(&(s->special_mask), 5, 1);
	}
	//num lock: toggle behaviour
	if (is_bit(s->special_mask, 6) && scancode == special[6].byte1) { //makecode
		set_bit(&(s->special_mask), 6, 0);
	} else if (!is_bit(s->special_mask, 6) && scancode == special[6].byte1) { //makecode
		set_bit(&(s->special_mask), 6, 1);
	}
}

int isCTRL(special_key_t * s) {
	return is_bit(s->special_mask, 0);
}
int isALT(special_key_t * s) {
	return is_bit(s->special_mask, 1);
}
int isSHIFT(special_key_t * s) {
	return is_bit(s->special_mask, 2) || is_bit(s->special_mask, 3);
}
int isALTGR(special_key_t * s) {
	return is_bit(s->special_mask, 4);
}
int isCAPSLOCK(special_key_t * s) {
	return is_bit(s->special_mask, 5);
}
int isNUMLOCK(special_key_t * s) {
	return is_bit(s->special_mask, 6);
}

key * getLETTER(code_t * c) {
	if (c->byte1 == KBD_ONE_BYTE) {
		int i;
		for (i = 0; i < KEYS_SIZE; i++) {
			if (keys[i].hex == c->byte2) {
				key * temp = malloc(sizeof(key));
				(*temp) = keys[i];
				return temp;
			}
		}
	}
	return NULL;
}

//-------------------------------------CODE
code_t * new_code(unsigned long b1, unsigned long b2) {
	code_t *c = malloc(sizeof(code_t));
	c->byte1 = b1;
	c->byte2 = b2;
}

//------------------------------------kbd_tupple
kbd_tupple * new_kbd_tupple(code_t * c, KbdAction * ka, key * k,
		special_key_t * s) {
	kbd_tupple * kt = malloc(sizeof(kbd_tupple));
	kt->code = c;
	kt->action = ka;
	kt->k = k;
	kt->special = s;
	return kt;
}

//-------------------------------------KBD interrupts
KbdAction * new_kbd_action(KbdAction val) {
	KbdAction * ka = malloc(sizeof(KbdAction));
	*ka = val;
	return ka;
}

KbdAction * get_action(special_key_t * s, code_t * c, key **k) {
	*k = NULL;
	if (isCTRL(s)) {
		if (c->byte1 == KBD_ONE_BYTE) {
			if (c->byte2 == KEY_C) {
				return new_kbd_action(COPY);
			} else if (c->byte2 == KEY_X) {
				return new_kbd_action(CUT);
			} else if (c->byte2 == KEY_V) {
				return new_kbd_action(PASTE);
			} else if (c->byte2 == KEY_Z) {
				return new_kbd_action(GO_BACK);
			} else if (c->byte2 == KEY_Y) {
				return new_kbd_action(GO_FORWARD);
			} else if (c->byte2 == KEY_D) {
				return new_kbd_action(DELETE_LINE);
			} else if (c->byte2 == KEY_O) {
				return new_kbd_action(OPEN_FILE);
			} else if (c->byte2 == KEY_S) {
				return new_kbd_action(SAVE);
			} else if (c->byte2 == KEY_E) {
				return new_kbd_action(STOP);
			} else if (c->byte2 == KEY_PLUS) {
				return new_kbd_action(BACKGROUND_NEXT);
			} else if (c->byte2 == KEY_MINUS) {
				return new_kbd_action(BACKGROUND_PREV);
			} else if (c->byte2 == KEY_N) {
				return new_kbd_action(NEW_FILE);
			}
		}
	} else if (isALT(s)) {
		if (c->byte1 == KBD_ONE_BYTE) {
			if (c->byte2 == KEY_PLUS) {
				return new_kbd_action(FONT_COLOR_NEXT); //exit
			} else if (c->byte2 == KEY_MINUS) {
				return new_kbd_action(FONT_COLOR_PREV); //exit
			}
		}
	}
	if (c->byte2 == KEY_ENTER) {
		return new_kbd_action(PARAGRAPH); //exit
	} else if (c->byte2 == KEY_UP || c->byte2 == KEY_LEFT
			|| c->byte2 == KEY_RIGHT || c->byte2 == KEY_DOWN) {
		return new_kbd_action(MOVE_CURSOR);
	} else if (c->byte2 == KEY_BACKSPACE) { //BACKSPACE, delete to the left
		return new_kbd_action(BACKSPACE);
	} else if (c->byte1 == KBD_TWO_BYTES && c->byte2 == KEY_DELETE) { //DELETE to the right
		return new_kbd_action(DELETE);
	}
	*k = getLETTER(c);
	if (*k != NULL) {
		return new_kbd_action(WRITE_CHAR);
	}
	return new_kbd_action(NOTHING);
}

int kbd_subscribe_int() {
	int mask = BIT(hook_id_kbd);
	if (sys_irqsetpolicy(KBD_IRQ, IRQ_EXCLUSIVE | IRQ_REENABLE, &hook_id_kbd)
			!= OK) {
		printf("sys_irqsetpolicy failed for keyboard!\n");
		return -1;
	}
	if (sys_irqenable(&hook_id_kbd) != OK) {
		printf("sys_irqenable failed for keyboard!\n");
		return -1;
	}
	return mask;
}

int kbd_unsubscribe_int() {
	if (sys_irqdisable(&hook_id_kbd) != OK) {
		printf("sys_irqdisable failed for keyboard!\n");
		return -1;
	}
	if (sys_irqrmpolicy(&hook_id_kbd) != OK) {
		printf("sys_irqenable failed for keyboard!\n");
		return -1;
	}
	return 0;
}

void print_special_key(special_key_t * s) {
	int i;
	for (i = 0; i < COUNT_SPECIAL; i++) { //-2 because of caps lock and num lock which have a persistent behaviour
	}
}

kbd_tupple * kbd_int_handler() {
	static unsigned short double_key = 0;
	unsigned long kbd_status, scancode;

	if (sys_inb(STATUS_REGISTER, &kbd_status) != OK) {
		printf("Unable to read kbd status\n");
		return NULL;
	}

	if (!(kbd_status & OUTPUT_BUFFER_FULL)) {
		printf("The output buffer is not full\n");
		return NULL;
	}

	if (sys_inb(OUTPUT_BUFFER, &scancode) != OK) {
		printf("Unable to read the scancode\n");
		return NULL;
	}

	update_special_key(&MASK, scancode, double_key);

	if (is_bit(scancode, 7)) {
		//printf("Breakcode: ");
	} else {
		//printf("Makecode: 0x%X\n", scancode);
	}
	if (KBD_TWO_BYTES != scancode) {
		code_t * c;
		key * k;
		if (double_key) {
			double_key = 0;
			c = new_code(KBD_TWO_BYTES, scancode);
			KbdAction * ka = get_action(&MASK, c, &k);
			return new_kbd_tupple(c, ka, k, &MASK);
		}
		c = new_code(KBD_ONE_BYTE, scancode);
		KbdAction * ka = get_action(&MASK, c, &k);
		return new_kbd_tupple(c, ka, k, &MASK);
	} else {
		double_key = 1;
	}

	return NULL;
}
