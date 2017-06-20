#ifndef __IO_H
#define __IO_H

#include "graphics.h"
#include "macros.h"
#include "keyboard.h"


/** @defgroup input_output input_output
 * @brief This is the I/O module, to handle files interface, display of chars, cursor and user actions that involve any of this
 * @{
 * Functions for the user interface and text display
 */

/**
 * @brief frame_t struct that maps one instance of the screen to optimize display functions in other modules
 */
typedef struct {
	char * frame;				/**@brief char * instance containing the pixel's information*/
	unsigned int width;			/**@brief dimension of screen, horizontally*/
	unsigned int height;		/**@brief dimension of screen, vertically*/
	unsigned char background;	/**@brief default background color of screen*/
	unsigned char font_color;	/**@brief default font color color of screen*/
} frame_t;

/**
 * @brief This function handles the creation of a new frame
 * allocating the needed memory for it
 * @param width The width of the new frame
 * @param height The height of the new frame
 * @param background The background of the new frame
 * @param font_color The color of the font of the new frame
 *
 * @return The pointer to the new frame
 */
frame_t * new_frame(unsigned int width, unsigned int height,
		unsigned char background, unsigned char font_color);

/**
 * @brief Create a vertical line in the frame
 * @param f The frame we want to draw the line in
 * @param x The 'x' position of the vertical line
 * @param y The 'y' position of the line
 * @param color The color we want to color the line in the frame
 *
 * @return 0 in success and 1 in failure
 */
int frame_pixel(frame_t * f, unsigned short x, unsigned short y,
		unsigned long color);

/**
 * @brief Display a pixmap in a certain position on screen
 * @param f The frame we are referring to
 * @param xi The initial 'x' position we want to draw the pixmap
 * @param yi The initial 'y' position we want to draw the pixmap
 * @param pm The pixmap pointer we want to display
 * @param width the Width of the pixmap
 * @param height The height of the pixmap
 *
 * @return 0 in success
 */
int frame_pixmap(frame_t * f, int xi, int yi, char* pm, int width, int height);

/**
 * @brief Frees the allocated memory of a frame
 * @param f The frame we wish to delete
 */
void delete_frame(frame_t * f);

/**
 * @brief Applies a frame to screen
 * @param The frame we wish to apply to the screen
 */
void frame_apply(frame_t * f);

/**
 *@brief Creates a line starting in (xi,yi) and finishing int (xf,yf) with a certain color
 *@param f The frame we are going to draw the line
 *@param xi The initial 'x' position of the line
 *@param yi The initial 'y' position of the line
 *@param xf The final 'x' position of the line
 *@param yf The final 'y' position of the line
 *@param color The color of the line we wish to draw
 *
 *@return 0 in success
 */
int frame_line(frame_t * f, unsigned short xi, unsigned short yi,
		unsigned short xf, unsigned short yf, unsigned long color);

/**
 *@brief Merges two screens into one
 *@param result The resulting frame
 *@param below The first frame
 *@param top The second frame
 *@param transparent transparent color
 *
 *@return 0 in failure and 1 in success
 */
int merge_frames(frame_t * result, const frame_t * below, const frame_t * top,
		char transparent);

/**
 * @brief Fills the frame with a certain color
 * @param f The frame we wih to fill with a color
 * @param color The color we are going to use
 */
void fill_frame(frame_t * f, unsigned int color);

/**
 *@brief Coppies one frame to another
 *@param destination The 'destination' frame
 *@param origin The 'original' frame
 *
 *@return 1 in success, 0 in failure
 */
int copy_frame(frame_t * destination, const frame_t * origin);

/**
 * @brief blinking cursor for the screen, this is handled as an isolated object with its own frame
 */
typedef struct {
	unsigned char state;	/**@brief show or hidden, this is changed every n miliseconds to give the blinking effect*/
	unsigned char enabled;	/**@brief toggle or not, useful for hiding the cursor*/
	unsigned int lineY;		/**@brief the line in the frame where it is shown*/
	unsigned int colX;		/**@brief the column in the frame where it is shown*/
	frame_t * f;			/**@brief the frame_t instance to draw the cursor in*/
} cursor_t;

/**
 * @brief Allocates memory for a new cursor (struct)
 * @param f The frame in which the cursor will be associated with
 *
 * @return pointer the the new allocated cursor memory
 */
cursor_t * new_cursor(frame_t * f);

/**
 * @brief Frees the memory allocated for the cursor
 * @param c The cursor we wish to free memory of
 */
void delete_cursor(cursor_t * c);

/**
 * @brief Toggles and draws the cursor on screen
 * @param c The cursor we want to toggle
 *
 * @return True if enabling the cursor was successful
 */
int toggle_cursor(cursor_t * c);

/**
 * @brief Draws the cursor on the screen
 * @param c The cursor we are referring to
 */
void draw_cursor(cursor_t * c);

/**
 * @brief screen struct to hold text in a frame, handles dynamic memory allocation for new/deleted chars
 */
typedef struct {
	unsigned long pos;			/**@brief the current position of the cursor*/
	unsigned long size; 		/**@brief the number of chars in the array*/
	unsigned long offset_top;	/**@brief how many lines including the first are not shown in the screen*/
	char clipboard;				/**@brief char to use in copy paste functions*/
	frame_t * f;				/**@brief the frame_t instance to draw the chars in*/
	cursor_t * cursor;			/**@brief cursor associated with the screen*/
	char * stream;				/**@brief array containing the chars*/
} screen_t;

/**
 * @brief Creates a new matrix with n lines, where the first element of
 * each line has the number of elements in that line (including itself)
 * @param width The width of the new screen
 * @param height The height of the new screen
 * @param background The background we want for the new screen
 * @param font_color The color we want for the new screen
 *
 * @return Pointer to the new allocated screen
 */
screen_t * new_screen(unsigned int width, unsigned int height,
		unsigned char background, unsigned char font_color);

/**
 * @brief Clears the memory allocated for a stream on a screen
 * @param s The screen we want to clear the stream from
 */
void clear_stream(screen_t * s);

/**
 * @brief Loads a string to the screen member 'stream'
 * @param s The screen we are referring to
 * @param string The new string we want to upload
 */
void load_string(screen_t * s, char * string);

/**
 * @brief Frees the allocated memory
 * @param s The screen we want to free memory from
 */
void delete_screen(screen_t * s);

/**
 * @bref Updates the current cursor position to stay up to date and
 * allow add_char and delete_char
 * @param s The screen's cursor we want to update
 * @param direction_key The direction to update the cursor
 */
void update_pos(screen_t * s, unsigned long direction_key);

/**
 * @brief Adds a char to the matrix, updates the column count (in the first column) and, if necessary,
 * moves the characters to the right to make space for the new one
 * @param s The screen we are referring to
 * @param c The new character to be added
 */
void add_char(screen_t * s, char c);

/**
 * @brief Deletes a char at pos and resizes the matrix
 * @param s The screen we are referring to
 * @param ka The action of the keyboard
 *
 * @return 1 on success, 0 otherwise
 */
int delete_char(screen_t * s, KbdAction ka);

/**
 * @brief Visually replaces a character on the screen, for example, when the user adds a char or deletes one.
 * Calculates the starting pixel from the line and pos and changes them in the v_mem
 * @param s The screen we are referring to
 * @param new_char The new character to be replaced
 * @param line The line near the character
 * @param pos The position on v_mem
 */
void replace_char(screen_t * s, unsigned char new_char, unsigned int line,
		unsigned int pos);

/**
 * @brief Increases the offset_top of the current screen (to show more lines below the current ones)
 * the function rewrite should be called afterwards
 * @param s The screen we are referring to
 */
void scroll_down(screen_t * s);

/**
 * @brief Decreases the offset_top of the current screen (to show more lines above the current ones)
 * the function rewrite should be called afterwards
 * @param s The screen we are referring to
 */
void scroll_up(screen_t * s);

/**
 * @brief Takes a screen a line and a position in that line and refreshes all the characters
 * after that position (for that line) and in all the lines until the end
 * Uses the replace_char function for each char
 * @param s The screen we are referring to
 * @param line The line in the text
 * @param pos Position which after, we want to replace
 */
void replace_screen(screen_t * s, unsigned int line, unsigned int pos);

/**
 * @brief Clears every char in the screen (replaces them by a space char which is VISUALLY equal to no char)
 * @see replace_char
 * @param s The screen we are referring to
 */
void empty_screen(screen_t * s);

/**
 * @brief Calls the vg_apply function by passing the current v_mem to display it in the screen
 * @see vg_apply
 * @param s The screen we are referring to
 */
void screen_show(screen_t * s);

/**
 * @brief Redraws all the characters in the current screen
 * @param s The screen we are referring to
 */
void redraw_screen(screen_t * s);

/**
 * @brief Changes the position of the cursor to new position
 * @param s The screen we are referring to
 * @param newLine The new line we want to change cursor to
 * @param newCol The new column we want to change cursor to
 */
void change_cursor_to(screen_t *s, int newLine, int newCol);

/**
 * https://stackoverflow.com/questions/2156572/c-header-file-with-bitmapped-fonts
 * http://stackoverflow.com/a/23130671
 */
void draw_char(frame_t * f, unsigned int offX, unsigned int offY, int ascii);

/**
 * @brief Checks if string is equal to the stream in the screen
 * @param s The screen we are referring to
 * @param string The string we want to compare
 * @param string_size The 'string' size
 *
 * @return 1 in success and 0 in failure
 */
int screen_string_comp(screen_t * s, char * string, unsigned int string_size);

/**
 * @brief Finds the previous paragraph from the one we are in
 * @param s The screen we are referring to
 * @param from_pos The position we are in
 *
 * @return The position of previous paragraph
 */
long find_previous_paragraph(screen_t * s, long from_pos);

/**
 * @brief Finds the next paragraph from the one we are in
 * @param s The screen we are referring to
 * @param from_pos The position we are in
 *
 * @return The position of next paragraph
 */
long find_next_paragraph(screen_t * s, long from_pos);

/**
 * @brief Finds the previous paragraph from the one we are in, ignoring the current one
 * @param s The screen we are referring to
 * @param from_pos The position we are in
 *
 * @return The position of next paragraph
 */
long find_next_paragraph_ignore_current(screen_t * s, long from_pos);

/**
 * @brief file_t struct to combine a screen_t struct with an actual text file
 * @see screen_t
 */
typedef struct {
	unsigned char saved;	/**@brief wheter there are any unsaved changes in the file*/
	char * filename; 		/**@brief Filename cstring, must end with '\0'*/
	screen_t * screen;		/**@brief screen_t instance to display and update file contents*/
} file_t;

/**
 * @brief Allocates memory for a new file structure
 * @param filename The name of the text file
 * @param s The screen we want to add
 * @return The pointer to a new allocated file structure
 */
file_t * new_file(char * filename, screen_t * s);

/**
 * @brief Frees the memory associated with a file
 * @param f The file we are referring to
 */
void delete_file(file_t * f);

/*
 * @brief Writes the stream into a file
 * @param f The file we want to write in
 *
 * @return 1 in success and 0 in failure
 */
int write_file(file_t * f);

/*
 * @brief Read a file into an object of file_t, places the cursor at the end
 * @param f The file we want to read from
 *
 * @return 1 on success, 0 otherwise
 */
int read_file(file_t * f);

/**
 * @brief Checks if the file named 'filename' exists
 * @param filename The name of the file
 *
 * @return 1 on success and 0 in failure
 */
int file_exists(char * filename);

/**
 * @brief create new file if it does not exist yet
 * @param f, a file_t pointer whose filename is the name of the new file
 *
 * @return 1 on success, 0 on failure and -1 if the file already exists
 */
int create_new_file(file_t * f);

/**
 * @brief Converts a stream (from the screen) into a string
 * @param s The screen we are referring to
 *
 * @return Pointo to char array of the converted string
 */
char * stream_to_cstring(screen_t * s);

/*
 * @brief Gets the next color in the 8 bit color pallete
 * @param color the current color
 *
 * @return The next color
 */
unsigned char get_next_color(unsigned char color);

/*
 * @brief Gets the previous color in the 8 bit color pallete
 * @param color the current color
 *
 * @return The previous color
 */
unsigned char get_previous_color(unsigned char color);

/**
 * @brief message_t struct to display a vibrating and disappearing toast in the top right corner of the screen
 */
typedef struct {
	unsigned short state;				/**@brief this takes values 0, 1 and 2 to show it vibrating, there are three possible positions: 0,1,2*/
	unsigned int duration;				/**@brief the current value passed since the display start in miliseconds*/
	unsigned int duration_vibrate;		/**@brief the current value passed since the vibration start in miliseconds*/
	unsigned int original_duration;		/**@brief total duration of message display in miliseconds*/
	unsigned int original_vibration;	/**@brief vibration total duration of message in miliseconds*/
	char * message;						/**@brief the cstring containing the message to display*/
	frame_t * f;						/**@brief the frame_t instance to draw the message in*/
} message_t;

/**
 * @brief Allocates memory for a new message (struct)
 * @param width Width of the message
 * @param height Height of the message
 * @param background Background of the message
 * @param font_color The color of the font
 * @param duration The duration of the message
 * @param duration_vibrate The duration of the vibration of the message
 * @param message The message itself
 *
 * @return Pointer to the new allocated memory of the message
 */
message_t * new_message(unsigned int width, unsigned int height,
		unsigned char background, unsigned char font_color,
		unsigned int duration, unsigned int duration_vibrate, char * message);

/**
 * @brief Frees the memory associated with a message
 * @param m The message we want to 'free'
 */
void delete_message(message_t * m);

/**
 * @brief Resets message content with new, with original duration
 * and vibration duration
 * @param m The message we want to reset
 * @param message The new message text to apply
 */
void reset_message(message_t * m, char * message);

/**
 * @brief Makes a message vibrate
 * @param m The message we want to vibrate
 *
 * @return 1 on success and 0 on failure
 */
int tick_message(message_t * m);

/**
 * @brief Changes the value of duration of a message to 0,
 * making it invisible
 * @param m The message we are referring to
 */
void hide_message(message_t * m);

/**
 * @brief Prints on screen the message
 * @param m The message we are referring to
 */
void draw_message(message_t * m);

/**
 * @brief Gets the size of a string
 * @param string The string we want to know the size of
 * @return The size of the string
 */
int cstring_size(char * string);

#endif /* __IO_H */
