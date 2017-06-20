#include <minix/syslib.h>
#include "main.h"
#include "macros.h"
#include "io.h"
#include "graphics.h"
#include "keyboard.h"
#include "timer.h"
#include "mouse.h"


void restart_graphics(){
	unsigned int width, height;
	void * v_mem = vg_init(GRAPHICS_MODE, &width, &height);
	vg_exit();
}


int subscriber(int *irq_set_ps2, int * irq_set_kbd, int * irq_set_timer) {
	if (((*irq_set_ps2) = mouse_subscribe_int()) == -1) {
		printf("unable to subscribe mouse\n");
		return 0;
	}
	if (((*irq_set_kbd) = kbd_subscribe_int()) == -1) {
		printf("unable to subscribe keyboard\n");
		return 0;
	}
	if (((*irq_set_timer) = timer_subscribe_int()) == -1) {
		printf("unable to subscribe timer\n");
		return 0;
	}
}

int unsubscriber() {
	if (mouse_unsubscribe_int() != 0) {
		printf("unable to unsubscribe mouse\n");
		return 0;
	}
	if (kbd_unsubscribe_int() != 0) {
		printf("unable to unsubscribe keyboard\n");
		return 0;
	}
	if (timer_unsubscribe_int() != 0) {
		printf("unable to unsubscribe timer\n");
		return 0;
	}
	if (vg_exit() != 0) {
		return 0;
		printf("unable to exit graphics mode\n");
	}
}

int merge_display(frame_t * result, frame_t * base, frame_t * cursor,
		frame_t * message, frame_t * mouse) {
	char transparent = (char) TRANSPARENT;
	unsigned int i, j;
	for (i = 0; i < result->height; i++) {		//merges the screens into one
		for (j = 0; j < result->width; j++) {
			if (mouse->frame[i * result->width + j] != transparent) {
				result->frame[i * result->width + j] = mouse->frame[i
						* result->width + j];
			} else if (message->frame[i * result->width + j] != transparent) {
				result->frame[i * result->width + j] = message->frame[i
						* result->width + j];
			} else if (cursor->frame[i * result->width + j] != transparent) {
				result->frame[i * result->width + j] = cursor->frame[i
						* result->width + j];
			} else if (base->frame[i * result->width + j] != transparent) {
				result->frame[i * result->width + j] = base->frame[i
						* result->width + j];
			} else {
				result->frame[i * result->width + j] = result->background;
			}
		}
	}

	frame_apply(result);
}

int main_editor(unsigned char background, unsigned char font_color,
		unsigned int auto_save, char *initial_file_name,
		unsigned char load_file) {
//Variables
	State state = WELCOME_MODE;					//initial state machine state
	FileNameState file_state = DISPLAY_MESSAGE;	//initial file state machine state
	FileNameState next_file_state = CHECK_IF_EXISTS;//to distinguish between new file and open existing file

	unsigned int width, height;						//dimensions of the screen
	void * v_mem = vg_init(GRAPHICS_MODE, &width, &height);	//v_mem address, initializes graphics mode

	int ipc_status, irq_set_kbd, irq_set_timer, irq_set_ps2, r;	//interrupts variables
	message msg;								//interrupts message variable
	unsigned char proceed = 1;					//while loop control variable

	unsigned long tempo_counter = 0;					//blinking cursor timer
	unsigned long auto_save_counter = 0;
	unsigned char file_chosen = 0;//boolean to check if a filename has been choosen
	unsigned char timer_interrupt = 0;//boolean to separate dispatcher from interrupt receiver
	unsigned char mouse_interrupt = 0;//boolean to separate dispatcher from interrupt receiver

//Structs initialization
	kbd_tupple * kt = NULL;	//to receive information from the keyboard in a tupple

	message_t * toast = new_message(width, height,
	BACKGROUND, FONT_COLOR, 180, 20, "");//toast message with background, font color, duration 180, vibration duration 20, "" as content
	hide_message(toast);							//hide the message created
	screen_t * io = new_screen(width, height, background, font_color);//screen for user input and output
	frame_t * main_frame = new_frame(width, height, background, font_color);//main frame, that holds the result of merging merged_frame and mouse_frame
	file_t * file = new_file(initial_file_name,
			new_screen(width, height, background, font_color));	//file instance to display the content and handle all the file's operations
	mouse_t *m = new_mouse(width, height, background, font_color);//mouse_t instance to display the mouse and capture its actions

//Display welcome message
	load_string(io, "Welcome to Minix Text Editor!\n\n"
			"Brought to you by Group 14 Class 2\n\n\n"
			".Ilona Generalova\n\n"
			".Miguel Ramalho\n\n\n"
			"Press Enter to continue.");//welcome screen message with credits
	redraw_screen(io);						//draw the message on the io frame
	merge_display(main_frame, io->f, io->f, toast->f, m->frame);//hide the cursor

//Subscribe interrupts
	subscriber(&irq_set_ps2, &irq_set_kbd, &irq_set_timer);	//subscribe the 3 interrupts mouse, kbd, timer

//Program flow
	while (proceed) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {//receive interrupts
			printf("driver_receive failed with: %d\n", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
//Receive interrupts
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set_kbd) {			//Keyboard interrupt
					kt = kbd_int_handler();	//get keyboard tupple from interrupt handler
				} else if (msg.NOTIFY_ARG & irq_set_ps2) {
					mouse_interrupt = ps2_int_handler(m);//update mouse position from interrupt handler
				} else if (msg.NOTIFY_ARG & irq_set_timer) {//Timer interrupt
					timer_interrupt = 1;
				}
				break;
			}

//Handle interrupts - Dispatcher
			if (timer_interrupt || mouse_interrupt) {
				mouse_interrupt = 0;
				if (timer_interrupt) {
					timer_interrupt = 0;
					tempo_counter++;
					auto_save_counter++;
					tick_message(toast);//update the message (toast) countdown value
					if (auto_save_counter == (auto_save * 60) && auto_save > 0
							&& state == EDIT_MODE) {//if the autosave is set and the file is in saving mode
						auto_save_counter = 0;
						if (file->saved) {
							reset_message(toast, "Auto save: No changes");
						} else {
							if (write_file(file)) {				//save changes
								reset_message(toast, "Auto save:  SAVED");
							} else {
								reset_message(toast, "Auto save: ERROR SAVING");
							}
						}
					}
					if (tempo_counter >= 30 && state == EDIT_MODE) {
						tempo_counter = 0;
						toggle_cursor(file->screen->cursor); //if the cursor is enabled blink it
					} else if (tempo_counter >= 30 && state == INPUT_MODE
							&& file_state == RECEIVE_FILENAME) {
						tempo_counter = 0;
						toggle_cursor(io->cursor); //if the cursor is enabled blink it
					}
				} else if (m->l_button) {				//left click with mouse
					m->l_button = 0;
					io->cursor->state = file->screen->cursor->state = 1;
					printf("change_cursor_to(file->screen, %d, %d);\n",
							get_mouse_line(m), get_mouse_column(m));
					change_cursor_to(file->screen, get_mouse_line(m),
							get_mouse_column(m));
				}
				if (state == EDIT_MODE) {
					merge_display(main_frame, file->screen->f,
							file->screen->cursor->f, toast->f, m->frame);
				} else {
					merge_display(main_frame, io->f, io->cursor->f, toast->f,
							m->frame);
				}
			} else if (kt != NULL) {//if there are keyboard commands to interpret
				if (kt->code->byte2 == KEY_ESC) {			//abort the program
					proceed = 0;
					continue;
				}

				switch (state) {				//state machine implementation
				case INPUT_MODE://user is writing information, such as the filename of the file to open
					switch (file_state) {//state machine to read valid filename
					case DISPLAY_MESSAGE:
						if (load_file) {//the file was specified in the arguments and is valid
							load_file = 0;
							read_file(file);
							state = EDIT_MODE;			//go to the edition mode
							redraw_screen(file->screen);
							merge_display(main_frame, file->screen->f,
									file->screen->cursor->f, toast->f,
									m->frame);
							continue;
						}
						if (*(kt->action) == PARAGRAPH) {//if the user presses enter then go to the input state
							file_state = RECEIVE_FILENAME;		//new file_state
							load_string(io, "home/lcom/svn/proj/src/");	//the default value of the filename, this is the path to the current folder
							redraw_screen(io);				//display the value
							merge_display(main_frame, io->f, io->cursor->f,
									toast->f, m->frame);
						} else {
							load_string(io, "Enter the filename (absolute"
									" path) after pressing enter.");//message the user to press enter to continue
							redraw_screen(io);
							merge_display(main_frame, io->f, io->cursor->f,
									toast->f, m->frame);
						}
						break;

					case RECEIVE_FILENAME://allow the user to write the file name
						io->cursor->enabled = 0;
						unsigned short refresh = 0;	//whether the screen needs to be updated
						if (*(kt->action) == PARAGRAPH) {
							refresh = 0;
							file_state = next_file_state;//CREATE_NEW OR CHECK_IF_EXISTS
						} else if (*(kt->action) == WRITE_CHAR
								&& kt->k != NULL) {	//the user is writing the chars in the filename
							refresh = 1;
							if (isSHIFT(kt->special)) {
								add_char(io, kt->k->upper);
							} else if (isALTGR(kt->special)) {
								add_char(io, kt->k->altgr);
							} else {
								add_char(io, kt->k->lower);
							}
						} else if (*(kt->action) == MOVE_CURSOR //the user is changing the cursor position
						&& kt->code != NULL) {
							refresh = 1;
							update_pos(io, kt->code->byte2);
						} else if (*(kt->action) == DELETE
								|| *(kt->action) == BACKSPACE) {//the user is pressing delete or backspace
							refresh = 1;
							delete_char(io, *(kt->action));
						} else if (*(kt->action) == OPEN_FILE) {//Ctrl+O - close the current file and open a new one
							next_file_state = CHECK_IF_EXISTS;
							file_state = DISPLAY_MESSAGE;
						} else if (*(kt->action) == NEW_FILE) {	//Ctrl+N - close the current file and create a new one
							next_file_state = CREATE_NEW;
							file_state = DISPLAY_MESSAGE;
						}

						if (refresh) {
							redraw_screen(io);			//recreate the screen
							tempo_counter = 30;
							io->cursor->state = 1;//show cursor while there is input
							draw_cursor(io->cursor);
							merge_display(main_frame, io->f, io->cursor->f,
									toast->f, m->frame);
						}

						io->cursor->enabled = 1;
						break;

					case CHECK_IF_EXISTS:
						;								//the ; is on purpose
						file->filename = stream_to_cstring(io);	//get a c-string from the screen I/O
						if (file_exists(file->filename)) {//if this is a valid file than load its information
							clear_stream(file->screen);
							read_file(file);
							state = EDIT_MODE;			//go to the edition mode
							redraw_screen(file->screen);
							merge_frames(main_frame, file->screen->f,
									file->screen->cursor->f, TRANSPARENT);
						} else {		//if this is not a valid file ask again
							load_string(io,
									"File not Found: Enter the filename (absolute path) after pressing enter.");
							redraw_screen(io);
							merge_display(main_frame, io->f, io->f, toast->f,
									m->frame);				//hide the cursor
						}
						file_state = DISPLAY_MESSAGE;
						break;

					case CREATE_NEW:						//create new file
						file->filename = stream_to_cstring(io);	//get a c-string from the screen I/O
						int result_create = create_new_file(file);
						if (result_create == 1) {//if this is a valid file than load its information
							reset_message(toast, "File created");
							state = EDIT_MODE;
							clear_stream(file->screen);
							read_file(file);
							redraw_screen(file->screen);
							merge_display(main_frame, file->screen->f,
									file->screen->cursor->f, toast->f,
									m->frame);
						} else if (result_create == -1) {//if this is not a valid file ask again
							load_string(io,
									"File Already Exists: Enter the filename (absolute path) for the new file after pressing enter.");
							redraw_screen(io);
							merge_display(main_frame, io->f, io->f, toast->f,
									m->frame);				//hide the cursor
						} else if (result_create == 0) {//unexpected error on creating file
							load_string(io,
									"Unable to create new file, check your permissions, press enter to try again\n");
							redraw_screen(io);
							merge_display(main_frame, io->f, io->f, toast->f,
									m->frame);				//hide the cursor
						}
						file_state = DISPLAY_MESSAGE;
						break;
					}
					break;

				case EDIT_MODE:				//state for editing the current file
					file->screen->cursor->enabled = 0;//disable the blinking cursor while the user is writing or deleting
					unsigned short refresh = 0;	//whether the screen needs to be updated

					//input operations
					if (*(kt->action) == PARAGRAPH) {//the user inserts a new paragraph
						refresh = 1;
						file->saved = 0;
						add_char(file->screen, '\n');
						hide_message(toast);
					} else if (*(kt->action) == WRITE_CHAR && kt->k != NULL) {//the user writes a char to the file
						refresh = 1;
						file->saved = 0;
						if (isSHIFT(kt->special)) {
							add_char(file->screen, kt->k->upper);
						} else if (isALTGR(kt->special)) {
							add_char(file->screen, kt->k->altgr);
						} else {
							add_char(file->screen, kt->k->lower);
						}
						hide_message(toast);
					} else if (*(kt->action) == DELETE
							|| *(kt->action) == BACKSPACE) {//the user is pressing delete or backspace
						refresh = 1;
						file->saved = 0;
						delete_char(file->screen, *(kt->action));
						hide_message(toast);
					} else if (*(kt->action) == MOVE_CURSOR
							&& kt->code != NULL) {//the user is changing the cursor position
						refresh = 1;
						update_pos(file->screen, kt->code->byte2);
					}

					//copy paste operations
					else if (*(kt->action) == COPY) {
						reset_message(toast, "COPIED");
						refresh = 1;
						file->saved = 0;
						file->screen->clipboard =
								file->screen->stream[file->screen->pos];//update the clipboard
					} else if (*(kt->action) == CUT) {
						reset_message(toast, "CUT");
						refresh = 1;
						file->saved = 0;
						file->screen->clipboard =
								file->screen->stream[file->screen->pos];//update the clipboard
						delete_char(file->screen, DELETE);//copy and remove = cut
					} else if (*(kt->action) == PASTE) {
						reset_message(toast, "PASTED");
						refresh = 1;
						file->saved = 0;
						add_char(file->screen, file->screen->clipboard);//add the char in the clipboard
					}

					//file operations
					else if (*(kt->action) == SAVE) {
						if (write_file(file)) {					//save changes
							reset_message(toast, "SAVED");
						} else {
							reset_message(toast, "ERROR SAVING");
						}
					} else if (*(kt->action) == OPEN_FILE) {//close the current file and open a new one
						next_file_state = CHECK_IF_EXISTS;
						reset_message(toast, "OPEN");
						if (file->saved) {//if the file has nothing to save show interface to open a new file
							state = INPUT_MODE;
							file_state = DISPLAY_MESSAGE;
						} else {//if the file is not saved, ask the user if he/she wants to save it
							state = DISPLAY_MODE;
						}
					} else if (*(kt->action) == NEW_FILE) {	//close the current file and create a new one
						next_file_state = CREATE_NEW;
						reset_message(toast, "NEW FILE");
						if (file->saved) {//if the file has nothing to save show interface to open a new file
							state = INPUT_MODE;
							file_state = DISPLAY_MESSAGE;
						} else {//if the file is not saved, ask the user if he/she wants to save it
							state = DISPLAY_MODE;
						}
					}

					//customization operations
					else if (*(kt->action) == BACKGROUND_NEXT) {//update all the frame's backgrounds to the next color
						refresh = 1;
						io->f->background = main_frame->background =
								file->screen->f->background = get_next_color(
										file->screen->f->background);
					} else if (*(kt->action) == BACKGROUND_PREV) {//update all the frame's backgrounds to the previous color
						refresh = 1;
						io->f->background = main_frame->background =
								file->screen->f->background =
										get_previous_color(
												file->screen->f->background);
					} else if (*(kt->action) == FONT_COLOR_NEXT) {//update all the frame's font_colors to the next color
						refresh = 1;
						io->cursor->f->font_color =
								file->screen->cursor->f->font_color =
										io->f->font_color =
												file->screen->f->font_color =
														get_next_color(
																file->screen->f->font_color);
					} else if (*(kt->action) == FONT_COLOR_PREV) {//update all the frame's font_colors to the previous color
						refresh = 1;
						io->cursor->f->font_color =
								file->screen->cursor->f->font_color =
										io->f->font_color =
												file->screen->f->font_color =
														get_previous_color(
																file->screen->f->font_color);
					}

					if (refresh) {
						redraw_screen(file->screen);	//recreate the screen
						tempo_counter = 30;
						file->screen->cursor->state = 1;//show cursor while there is input
						draw_cursor(file->screen->cursor);
						merge_display(main_frame, file->screen->f,
								file->screen->cursor->f, toast->f, m->frame);
					}

					file->screen->cursor->enabled = 1;
					break;

				case DISPLAY_MODE://prompt to save the file changes before proceeding
					load_string(io,
							"This file has unsaved changes, do you wish to "
									"save them before proceeding (y, n)?");
					redraw_screen(io);
					merge_display(main_frame, io->f, io->f, toast->f, m->frame);//hide the cursor
					if (*(kt->action) == WRITE_CHAR && kt->k != NULL) {
						unsigned char user_has_choosen = 0;
						if (kt->k->lower == 'y') {				//user pressed y
							write_file(file);					//save changes
							user_has_choosen = 1;
						} else if (kt->k->lower == 'n') {		//user pressed n
							user_has_choosen = 1;
						}
						if (user_has_choosen) {	//user pressed y or n, so this state will be exited
							state = INPUT_MODE;
							file_state = DISPLAY_MESSAGE;
							clear_stream(file->screen);
							file->saved = 1;
						}
					}
					break;
				case WELCOME_MODE:				//display instructions on start
					if (*(kt->action) == PARAGRAPH) {//enter takes the user to the next screen
						state = INPUT_MODE;
					}
					break;
				}
			}
			kt = NULL;	//make the keyboard tupple null for the next iteration
		}
	}

//Unsubscribe interrupts
	unsubscriber();

//Clear dynamically allocated memory
	delete_mouse(m);
	delete_screen(io);
	delete_frame(main_frame);

	file->filename = "clear";
	delete_screen(file->screen);
	free(file);

	delete_message(toast);
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n                         Minix Text Editor Completion!\n\n\n\n\n\n\n\n\n\n\n\n\n");
	return 1;
}

