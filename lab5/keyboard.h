#ifndef __KEYBOARD_H
#define __KEYBOARD_H

/**
 * @brief Check if bit is 1 in a byte
 *
 * @param c char that holds the bits
 * @param pos position to check
 * @return Return 1 if bit is 1 and 0 if not
 */
int is_bit (unsigned char c, int pos);

/**
 * @brief Subscribes and enables KBD interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int kbd_subscribe_int(void);

/**
 * @brief Unsubscribes KBD interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_unsubscribe_int();

/**
 * @brief Tests KBD interrupt handling
 *
 *
 * @param ass Which interrupt handler to use
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_test_int(unsigned long ass);


/**
 * @brief KBD interrupt handler
 *
 * Checks and reads from the buffer a single scancode, then prints it
 */
unsigned long kbd_int_handler();

/**
 * @brief output scancode
 *
 * Checks whether the scancode is a makecode or breakcode
 * @param scancode - the scancode to analyze ad output
 */
void print_scancode(unsigned long scancode);


/**
 * @brief Write KBC Command
 *
 * Tries for a number of time before aborting
 * @param cmd - the command to send to the COMMAND_REGISTER
 * @return int 0 on success, -1 on failure
 */
int write_to_kbc(unsigned long cmd);

/**
 * @brief Read response from OUTPUT_BUFFER
 *
 * Tries for a number of time before aborting
 * @param data - pointer to variable where the read command should be saved
 * @return int 0 on success, -1 on failure
 */
int read_from_kbc(unsigned long *data);

#endif /* __KEYBOARD.H */
