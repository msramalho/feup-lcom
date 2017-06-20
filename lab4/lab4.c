#include <limits.h>
#include <string.h>
#include <errno.h>
#include "test4.h"
#include "mouse.h"
#include "timer.h"
#include "i8042.h"

static void print_usage(char **argv);
static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);

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
			"\t service run %s -args \"packets <int - number of packets to process>\"\n"
			"\t service run %s -args \"async <int - number of seconds in idle before stopping>\"\n"
			"\t service run %s -args \"config\"\n"
			"\t service run %s -args \"gesture\"\n",
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv){
	unsigned short packets, idle_seconds;
	short int gesture_length;

	if (strncmp(argv[1], "packets", strlen("packets")) == 0) {
		if (argc != 3) {
			printf("ps/2 packets: wrong no. of arguments for test_packet()\n");
			return 1;
		}
		packets = parse_ulong(argv[2], 10);

		if (packets == ULONG_MAX)
			return 1;
		printf("ps/2::test_packet(%lu)\n", packets);
		return test_packet(packets);
	}else if (strncmp(argv[1], "async", strlen("async")) == 0) {
		if (argc != 3) {
			printf("ps/2 async: wrong no. of arguments for test_async()\n");
			return 1;
		}
		idle_seconds = parse_ulong(argv[2], 10);

		if (idle_seconds == ULONG_MAX)
			return 1;
		printf("ps/2::test_async(%lu)\n", idle_seconds);
		return test_async(idle_seconds);
	}else if (strncmp(argv[1], "config", strlen("config")) == 0) {
		if (argc != 2) {
			printf("ps/2 config: wrong no of arguments for test_config()\n");
			return 1;
		}
		printf("ps/2::test_config()\n");
		return test_config();
	}else if (strncmp(argv[1], "gesture", strlen("gesture")) == 0) {
		if (argc != 3) {
			printf("ps/2 gesture: wrong no of arguments for test_gesture()\n");
			return 1;
		}
		gesture_length = strtol(argv[2], NULL, 10);

		printf("ps/2::test_gesture(%lu)\n", gesture_length);
		return test_gesture(gesture_length);
	}else {
		printf("ps/2: %s - no valid function!\n", argv[1]);
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
		printf("timer: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}
