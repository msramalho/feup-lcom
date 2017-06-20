#ifndef __UTILS_H
#define __UTILS_H

#include <limits.h>
#include <errno.h>
#include "io.h"


/** @defgroup utils utils
 * @brief This is the utils module, this module has general functions whose use may happen among many other modules
 * @{
 * Functions for the utils interface
 */

/**
 * @brief parse unsigned long from string
 * @param str the original string
 * @param base the base of the number to be returned
 */
unsigned long parse_ulong(char *str, int base);

/**
 * @brief parse long from string
 * @param str the original string
 * @param base the base of the number to be returned
 */
unsigned long parse_long(char *str, int base);

/**
 * @brief Check if bit is 1 in a byte
 *
 * @param c char that holds the bits
 * @param pos position to check
 * @return Return 1 if bit is 1 and 0 if not
 */
int is_bit(unsigned char c, int pos);

/**
 * @brief Set a bit to a certain value
 *
 * @param c char that holds the bit to change
 * @param pos position to change
 * @param value 1 to set the bit and 0 to clear the bit
 */
void set_bit(unsigned char *c, int pos, int value);

/**
 * @brief Checks if number is lower than the other
 * @param a First number
 * @param b Second Number
 * @return 1 on true, 0 on false
 */
int min(unsigned long a, unsigned long b);

/**
 * @brief Checks if number is bigger than the other
 * @param a First number
 * @param b Second Number
 * @return 1 on true, 0 on false
 */
int max(unsigned long a, unsigned long b);

/**
 * @brief sprite struct to handle mouse xpm
 */
typedef struct {
	int x, y;            /**@brief current sprite position*/
	int width, height;   /**@brief sprite dimensions*/
	char *map;           /**@brief the sprite pixmap (use read_xpm())*/
} Sprite;

/**
 * @brief Allocates memory for a new sprite
 * @param pic The char * for the xpm
 * @param x The 'x' position of the sprite
 * @param y The 'y' position of the sprite
 * @param f The frame we want to associate the sprite
 */
Sprite * create_sprite(char *pic[], unsigned int x, unsigned int y,
		frame_t * f);

#endif /* __UTILS_H */
