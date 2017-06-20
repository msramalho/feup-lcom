#ifndef __MOUSE_H
#define __MOUSE_H


/**
 * @brief Display a 3 byte packt in a user friendly way
 *
 */
void print_packet(unsigned char b1, unsigned char b2, unsigned char b3);


/**
 * @brief gesture interrupt handler
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int ps2_gesture_handler(int length);

/**
 * @brief Check if bit is 1 in a byte
 *
 * @param c char that holds the bits
 * @param pos position to check
 * @return Return 1 if bit is 1 and 0 if not
 */
int is_bit (unsigned char c, int pos);

/**
 * @brief Subscribes and enables PS/2 interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int ps2_subscribe_int(void);

/**
 * @brief Unsubscribes PS/2 interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int ps2_unsubscribe_int();


/**
 * @brief Mouse interrupt handler
 *
 * Checks, reads and saves a single byte from the buffer
 */
unsigned long ps2_int_handler(unsigned char *packet, unsigned short *counter, unsigned short *current_byte);


/**
 * @brief Mouse interrupt handler
 *
 * Checks and reads a single byte from the buffer
 */
unsigned long ps2_int_handler_no_memory();

/**
 * @brief Writes any command to the mouse (using write byte commad 0xD4)
 *
 * @param c the command to send
 * @return Return 0 upon success and non-zero otherwise
 */
int write_command_to_mouse(unsigned long c);

/**
 * @brief Calls write_command_to_mouse() a defined number of times before aborting the operation
 *
 * @param attempts number of times to try
 * @param c the command to send
 * @return Return 0 upon success and non-zero otherwise
 */
int write_cmd_attempts(unsigned long c, unsigned int attempts);

/**
 * @brief reads a byte from the status register
 *
 * @return the byte in question
 */
unsigned long read_from_out_buf();

/**
 * @brief reads a byte from the status register
 * @param b the byte to send
 * @param p the port to which the byte will be sent
 * @return Return 0 upon success and non-zero otherwise
 */
int write_byte_to_port(unsigned long p, unsigned long b);

int two_to_decimal(unsigned char bin, int negative);


#endif /* __MOUSE.H */
