#include <limits.h>
#include <string.h>
#include <errno.h>
#include "test5.h"
#include "timer.h"
#include "i8042.h"

#include "pixmap.h"


static void print_usage(char **argv);
static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static unsigned long parse_long(char *str, int base);
int main(int argc, char *argv[]) {
	sef_startup();
	printf("-------------------------------------------------------------------------------\n");
	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
	printf("DONE");
}

static void print_usage(char **argv){
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"init  <int - mode> <int - delay (in seconds)>\"\n"
			"\t service run %s -args \"square  <int - x> <int - y> <int - size> <long - color> \"\n"
			"\t service run %s -args \"line <int - xi> <int - yi> <int - xf> <int - yf> <long - color>\"\n"
			"\t service run %s -args \"xpm <int - xi> <int - yi> <xpm id number (1 to 5)>\"\n"
			"\t service run %s -args \"move <int - xi> <int - yi> <xpm id number (1 to 5)> <int - horizontal> "
			" <int - delta> <int - time (seconds)>\"\n"
			"\t service run %s -args \"controller\"\n",
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv){

	if (strncmp(argv[1], "init", strlen("init")) == 0) {
		unsigned long mode;
		unsigned short delay;
		if (argc != 4) {
			printf("vbe init: wrong no. of arguments for test_init(unsigned short mode, unsigned short delay)\n");
			return 1;
		}
		mode = parse_ulong(argv[2], 16);
		delay = parse_ulong(argv[3], 10);

		if (mode == ULONG_MAX || delay == ULONG_MAX)
			return 1;
		printf("vbe::test_init(0x%X, %lu)\n", mode, delay);
		return test_init(mode, delay) != NULL?0:1;


	}else if (strncmp(argv[1], "square", strlen("square")) == 0) {
		unsigned short x, y, size;
		unsigned long color;

		if (argc != 6) {
			printf("vbe square: wrong no. of arguments for test_square(unsigned short x,"
					" unsigned short y, unsigned short size, unsigned long color)\n");
			return 1;
		}
		x = parse_ulong(argv[2], 10);
		y = parse_ulong(argv[3], 10);
		size = parse_ulong(argv[4], 10);
		color = parse_ulong(argv[5], 16);

		if (x == ULONG_MAX || y == ULONG_MAX || size == ULONG_MAX || color == ULONG_MAX)
			return 1;
		printf("vbe::test_square(%lu, %lu, %lu, 0x%X)\n", x, y, size, color);
		return test_square(x, y, size, color);


	}else if (strncmp(argv[1], "line", strlen("line")) == 0) {
		unsigned short xi, yi, xf, yf;
		unsigned long color;

		if (argc != 7) {
			printf("vbe line: wrong no. of arguments for int test_line(unsigned short xi,"
					" unsigned short yi, unsigned short xf, unsigned short yf, unsigned long color)\n");
			return 1;
		}
		xi = parse_ulong(argv[2], 10);
		yi = parse_ulong(argv[3], 10);
		xf = parse_ulong(argv[4], 10);
		yf = parse_ulong(argv[5], 10);
		color = parse_ulong(argv[6], 16);

		if (xi == ULONG_MAX || yi == ULONG_MAX || xf == ULONG_MAX || yf == ULONG_MAX || color == ULONG_MAX)
			return 1;
		printf("vbe::test_line(%lu, %lu, %lu, %lu, 0x%X)\n", xi, yi, xf, yf, color);
		return test_line(xi, yi, xf, yf, color);


	}else if (strncmp(argv[1], "xpm", strlen("xpm")) == 0) {
		unsigned short xi, yi, selected_pixmap;
		char **xpm;

		if (argc != 5) {
			printf("ps/2 config: wrong no of arguments for test_xpm(unsigned short xi, unsigned short yi,"
					" char *xpm[])\n");
			return 1;
		}
		xi = parse_ulong(argv[2], 10);
		yi = parse_ulong(argv[3], 10);
		selected_pixmap = parse_ulong(argv[4], 10);
		if (xi == ULONG_MAX || yi == ULONG_MAX || selected_pixmap==ULONG_MAX)
			return 1;

		switch(selected_pixmap){
		case 1:
			xpm = pic1;break;
		case 2:
			xpm = pic2;break;
		case 3:
			xpm = cross;break;
		case 4:
			xpm = pic3;break;
		case 5:
			xpm = penguin;break;
		default:
			printf("vbe::test_xpm - STOPPED - INVALID XPM (1 to 5)\n");
		}

		printf("vbe::test_xpm(%lu, %lu, %lu)\n", xi, yi, xpm);
		return test_xpm(xi,yi,xpm);


	}else if (strncmp(argv[1], "move", strlen("move")) == 0) {
		unsigned short xi, yi, hor, time, selected_pixmap;
		short delta;
		char **xpm;

		if (argc != 8) {
			printf("vbe move: wrong no of arguments for test_move(unsigned short xi, unsigned short yi,"
					" char *xpm[], unsigned short hor, short delta, unsigned short time)\n");
			return 1;
		}
		xi = parse_ulong(argv[2], 10);
		yi = parse_ulong(argv[3], 10);
		selected_pixmap = parse_ulong(argv[4], 10);
		hor = parse_ulong(argv[5], 10);
		delta = parse_long(argv[6], 10);
		time = parse_ulong(argv[7], 10);


		if (xi == ULONG_MAX || yi == ULONG_MAX || hor == ULONG_MAX || time == ULONG_MAX || delta == LONG_MAX || selected_pixmap == ULONG_MAX)
			return 1;


		switch(selected_pixmap){
		case 1:
			xpm = pic1;break;
		case 2:
			xpm = pic2;break;
		case 3:
			xpm = cross;break;
		case 4:
			xpm = pic3;break;
		case 5:
			xpm = penguin;break;
		default:
			printf("vbe::test_move - STOPPED - INVALID XPM (1 to 5)\n");
		}

		printf("vbe::test_move(%lu, %lu, %lu, %lu, %lu, %lu)\n", xi, yi, xpm, hor, delta, time);
		return test_move(xi, yi, xpm, hor, delta, time);
	}else if(strncmp(argv[1], "controller", strlen("controller")) == 0){
		printf("vbe::test_controller()\n");
		return test_controller();
	}else if(strncmp(argv[1], "ascii", strlen("ascii")) == 0){
		printf("vbe::test_ascii()\n");
		return test_ascii();
	}else {
		printf("vbe: %s - no valid function!\n", argv[1]);
		return 1;
	}
}


static unsigned long parse_ulong(char *str, int base){
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

static unsigned long parse_long(char *str, int base) {
	char *endptr;
	long val;

	/* Convert string to long */
	val = strtol(str, &endptr, base);

	if ((errno == ERANGE && val == LONG_MAX ) || (errno != 0 && val == 0)) {
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
