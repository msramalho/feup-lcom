#ifndef __MOUSE_H
#define __MOUSE_H
#include "io.h"


/** @defgroup mouse mouse
 * @brief This is the mouse module, to handle all the mouse operations, such as packet handling and interrupts
 * @{
 * Functions for the mouse interface
 */

/**
 * @brief mouse_t struct to hold all the relevant information for the mouse
 */
typedef struct {
	unsigned char bytes[3];		/**@brief array holding the 3 bytes packet*/
	long x;						/**@brief horizontal position in screen*/
	long y;						/**@brief vertical position in screen*/
	unsigned char l_button;		/**@brief left button click boolean*/
	unsigned char m_button;		/**@brief middle button click boolean*/
	unsigned char r_button;		/**@brief right button click boolean*/
	unsigned char counter;		/**@brief register the current byte in the 3 byte packet*/
	frame_t *frame;				/**@brief frame_t instance to display the mouse*/
} mouse_t;

/**
 * @brief Creates a new mouse m
 * @return Pointer to the mouse m
 */
mouse_t *new_mouse(unsigned int width, unsigned int height,
		unsigned char background, unsigned char font_color); // "constructor"

/**
 * @brief Destroys the current mouse instance
 * @param m the mouse to be destroyed
 */
void delete_mouse(mouse_t *m);

/**
 * @brief Subscribes and enables PS/2 interrupts
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int mouse_subscribe_int();

/**
 * @brief Unsubscribes PS/2 interrupts
 * @return Return 0 upon success and non-zero otherwise
 */
int mouse_unsubscribe_int();

/**
 * @brief Mouse interrupt handler
 * Checks, reads and saves a single byte from the buffer
 */
int mouse_int_handler(mouse_t *m);

/**
 * @brief Checks mouse status and reads one byte from the output buffer
 * @return Returns data from the output buffer
 */
unsigned long mouse_read();

/**
 * @brief Draws the mouse cursor by creating a sprite
 * @param m mouse for which we want to draw the cursor
 */
void draw_mouse(mouse_t * m);

/**
 * @brief mouse pointer xpm;
 * https://wiki.libsdl.org/SDL_CreateCursor
 */
static char *mouse_xpm[] = {
"11 17 3",
"  0",
". 63",
"X 136",
".XXXXXXXXXX",
"..XXXXXXXXX",
". .XXXXXXXX",
".  .XXXXXXX",
".   .XXXXXX",
".    .XXXXX",
".     .XXXX",
".      .XXX",
".       .XX",
".        .X",
".     .....",
".  .  .XXXX",
". .X.  .XXX",
"..XX.  .XXX",
".XXXX.  .XX",
"XXXXX.  .XX",
"XXXXXX..XXX"
};


/**
 * @brief to obtain the mouse pointer xpm
 * @return xpm with the mouse pointer
 */
char **get_mouse_xpm();

/**
 * @brief Reads and saves information if mouse buttons are pressed,
 * checks if the mouse was moved and calculates the distance of moving,
 * doesn't let to move mouse out of screen
 * @param m mouse whose information we want to get
 */
static void get_mouse_info(mouse_t *m);

/**
 * @brief Transforms two complements to decimal
 * @param bin The bianry number
 * @param negative Flag if number is engative
 *
 * @return The converted decimal number
 */
int two_to_decimal(unsigned char bin, int negative);

/**
 * @brief Prints packets of the mouse
 * @param The mouse we are referring to
 */
void mouse_print_packet(mouse_t *m);

/**
 * @brief Writes any command to the mouse (using write byte commad 0xD4)
 * @param c the command to send
 * @return Return 0 upon success and non-zero otherwise
 */
int write_command_to_mouse(unsigned long c);

/**
 * @brief Calls write_command_to_mouse() a defined number of times before aborting the operation
 * @param attempts number of times to try
 * @param c the command to send
 * @return Return 0 upon success and non-zero otherwise
 */
int write_cmd_attempts(unsigned long c, unsigned int attempts);

/**
 * @brief Reads a byte from the status register
 * @param b the byte to send
 * @param p the port to which the byte will be sent
 * @return Return 0 upon success and non-zero otherwise
 */
int write_byte_to_port(unsigned long p, unsigned long b);

/**
 * @brief To get the number of text line where the mouse is located at the moment
 * @param m mouse whose location we want to get
 * @return the number of line
 */
int get_mouse_line(mouse_t *m);

/**
 * @brief To get the number of text column where the mouse is located at the moment
 * @param m mouse whose location we want to get
 * @return the number of column
 */
int get_mouse_column(mouse_t *m);

#endif /* __MOUSE_H */
