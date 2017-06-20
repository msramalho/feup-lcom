#include "utils.h"
#include "io.h"

unsigned long parse_ulong(char *str, int base) {
	char *endptr;
	unsigned long val;

	/* Convert string to unsigned long */
	val = strtoul(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == ULONG_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return ULONG_MAX;
	}

	if (endptr == str) {
		printf("error: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}

unsigned long parse_long(char *str, int base) {
	char *endptr;
	long val;

	/* Convert string to long */
	val = strtol(str, &endptr, base);

	if ((errno == ERANGE && val == LONG_MAX) || (errno != 0 && val == 0)) {
		perror("strtol");
		return LONG_MAX;
	}

	if (endptr == str) {
		printf("error: parse_ulong: no digits were found in %s \n", str);
		return LONG_MAX;
	}

	/* Successful conversion */
	return val;
}

int is_bit(unsigned char c, int pos) {
	return c >> pos & 0x01;
}

void set_bit(unsigned char *c, int pos, int value) {
	(*c) ^= (-value ^ (*c)) & (1 << pos);
}

int min(unsigned long a, unsigned long b) {
	return a < b ? a : b;
}
int max(unsigned long a, unsigned long b) {
	return a < b ? b : a;
}

Sprite * create_sprite(char *pic[], unsigned int x, unsigned int y, frame_t * f) {
	Sprite *s = (Sprite *) malloc(sizeof(Sprite));
	s->x = x;
	s->y = y;
	s->map = (char *) read_xpm(pic, &(s->width), &(s->height));
	if (frame_pixmap(f, x, y, s->map, s->width, s->height) != 0) {
		return NULL;
	}

	return s;
}
