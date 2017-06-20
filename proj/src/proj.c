#include <limits.h>
#include <string.h>
#include <errno.h>
#include "utils.h"

static void print_usage(char **argv);
static int proc_args(int argc, char **argv);

int main(int argc, char *argv[]) {
	sef_startup();
	lm_init();
	printf("-------------------------------------------------------------------------------\n");
	if (argc == 1) {
		print_usage(argv);
		return 0;
	} else
		return proc_args(argc, argv);
	printf("DONE");
}

static void print_usage(char **argv) {
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"test\"\n"
			"\t service run %s -args \"kbd\"\n", argv[0], argv[0]);
}

static int proc_args(int argc, char **argv) {
	if(strncmp(argv[1], "restart_graphics", strlen("restart_graphics")) == 0){
		restart_graphics();
	} else if (strncmp(argv[1], "default", strlen("default")) == 0) { //black and white, no autosave, no preselected file
		printf("main_editor(0, 15, 0, '', 0)\n");
		main_editor(0, 15, 0, "", 0);
	} else if (strncmp(argv[1], "save", strlen("save")) == 0) { //black and white, only autosave, no preselected file
		unsigned int auto_save; //time between autosave in seconds
		auto_save = (unsigned int) parse_ulong(argv[2], 10);

		printf("main_editor(0, 15, %d, '', 0)\n", auto_save);
		main_editor(0, 15, auto_save, "", 0);
	} else if (strncmp(argv[1], "main", strlen("main")) == 0) { //custom colors, autosave, and maybe preselected file
		printf("main_editor()\n");

		unsigned char background, font_color; //initial color set
		unsigned int auto_save; //time between autosave in seconds
		char* initial_file_name = ""; //file to open
		unsigned char load_file = 0; //if it is 0 then the user is prompted for the filename
		printf("argc = %d\n", argc);
		if (argc != 6 && argc != 5) {
			printf(
					"MINIX text editor: wrong no. of arguments for main_editor(unsigned char background,"
							" unsigned char font_color, unsigned int auto_save, char *initial_file_name, unsigned char load_file)\n");
			return 1;
		}
		background = (unsigned char) parse_ulong(argv[2], 10);
		font_color = (unsigned char) parse_ulong(argv[3], 10);
		auto_save = (unsigned int) parse_ulong(argv[4], 10);
		if (argc == 6) {
			load_file = 1;
			initial_file_name = argv[5];
			if (!file_exists(initial_file_name)) {
				printf("This filename is not valid!\n");
				return 1;
			}
		}

		printf("main_editor(%c, %c, %d, %s, %d)\n", background, font_color,
				auto_save, initial_file_name, (int) load_file);

		main_editor(background, font_color, auto_save, initial_file_name, load_file);

	} else {
		printf("MINIX text editor: %s - no valid function!\n", argv[1]);
		return 1;
	}
	return 0;
}

