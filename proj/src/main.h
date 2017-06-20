#ifndef __MAIN_H
#define __MAIN_H

#include "utils.h"

/** @defgroup main main
 * @brief This is the main module, to handle all the upper level operations, by calling the other modules
 * @{
 * Functions for the main interface
 */


/**
 * @brief State enum for the main state machine
 */
typedef enum State {
	EDIT_MODE, INPUT_MODE, DISPLAY_MODE, WELCOME_MODE
} State;

/**
 * @brief FileNameState enum for the sub-state machine used for the filename handling
 */
typedef enum FileNameState {
	DISPLAY_MESSAGE, RECEIVE_FILENAME, CREATE_NEW, CHECK_IF_EXISTS
} FileNameState;

/**
 * @brief function used to restart graphics mode after service stop
 */
void restart_graphics();

/**
 * @brief invokes all the required interrupt subscribers
 * Mouse, keyboard, timer
 * @param irq_set_ps2 the mouse interrupt identifier
 * @param irq_set_kbd the keyboard interrupt identifier
 * @param irq_set_timer the timer interrupt identifier
 */
int subscriber(int *irq_set_ps2, int * irq_set_kbd, int * irq_set_timer);

/*
 * @brief unsubscribes the subscribed interrupts in subscriber
 * @see subscriber
 */
int unsubscriber();

/*
 * @brief merge the five required frames into one and displays it on the screen
 * This frames are overlayed in the specified order, so that the mouse, for instance, stays over the text
 *
 * @param result a frame_t instance that holds the merging result, this way there is no need to create a new frame_t inside the function or even change the other ones pased
 * @param base a frame_t instance that is the one most underneath
 * @param cursor a frame_t instance that is the second most underneath
 * @param message a frame_t instance that is the third most underneath
 * @param mouse a frame_t instance that is the one that is always displayed (the pixels not transparent)
 */
int merge_display(frame_t * result, frame_t * base, frame_t * cursor,
		frame_t * message, frame_t * mouse);

/**
 * @brief main flow control function
 * Implements the initialization of the interrupts;
 * Handles the state machine flow, namely user prompts and file actions;
 * Handles mouse and keyboard actions and calls the necessary display functions
 * @param background the default background color (from 0 to 15) for the interface
 * @param font_color the default font color (from 0 to 15) for the interface
 * @param auto_save the seconds between each the file is automatically saved (0 means this is disabled)
 * @param initial_file_name the filename of the file to open
 * @param load_file, a boolean; if 0 then the initial_file_name is not considered and the user is prompted for a new filename, if 1 the initial_file_name is used
 * @return 0 on failure, 1 on success
 */
int main_editor(unsigned char background, unsigned char font_color,
		unsigned int auto_save, char *initial_file_name,
		unsigned char load_file);

#endif /* __MAIN_H */
