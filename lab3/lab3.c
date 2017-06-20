#include <limits.h>
#include <string.h>
#include <errno.h>
#include "test3.h"
#include "timer.h"
#include "keyboard.h"

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv)
{
	sef_startup();
	printf("-------------------------------------------------------------------------------\n");
	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
	printf("DONE");
}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"scan <bool - assembly mode>\"\n"
			"\t service run %s -args \"leds <decimal no. - number of leds, list of leds (0 = SCROLLLOCK, 1 = NUMLOCK, 2 = CAPSLOCK)>\"\n"
			"\t service run %s -args \"timed <decimal no. - seconds without input before exiting>\"\n",
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	unsigned short ass, count_leds, max_time_no_input;
	unsigned short * leds;
	int i;
	if (strncmp(argv[1], "scan", strlen("scan")) == 0) {
		if (argc != 3) {
			printf("kbd scan: wrong no. of arguments for kbd_test_scan()\n");
			return 1;
		}
		ass = parse_ulong(argv[2], 10);	/* Parses string to unsigned long */

		if (ass == ULONG_MAX)
			return 1;
		printf("kbd::kbd_test_scan(%lu)\n", ass);
		return kbd_test_scan(ass);
	}
	else if (strncmp(argv[1], "leds", strlen("leds")) == 0) {
		if (argc < 4) {
			printf("kbd: wrong no. of arguments for kbd_test_leds()\n");
			return 1;
		}
		count_leds = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		if (count_leds == ULONG_MAX)
			return 1;

		leds = (unsigned short *) malloc(sizeof(unsigned short) * count_leds);/*Dynamically allocating memory for the leds order*/

		for(i = 0; i < count_leds; i++){
			leds[i] = parse_ulong(argv[i + 3], 10);//parse each led from the args
			if (leds[i] == ULONG_MAX)
				return 1;
		}
		printf("kbd::kbd_test_leds(%lu, leds)\n", count_leds);
		return kbd_test_leds(count_leds, leds);
	}
	else if (strncmp(argv[1], "timed", strlen("timed")) == 0) {
		if (argc != 3) {
			printf("kbd: wrong no of arguments for kbd_test_timed_scan()\n");
			return 1;
		}
		max_time_no_input = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		if (max_time_no_input == ULONG_MAX)
			return 1;
		printf("kbd::kbd_test_timed_scan(%lu)\n", max_time_no_input);
		return kbd_test_timed_scan(max_time_no_input);
	}
	else {
		printf("kbd: %s - no valid function!\n", argv[1]);
		return 1;
	}
}

static unsigned long parse_ulong(char *str, int base)
{
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
		printf("timer: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}
