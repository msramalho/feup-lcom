#ifndef __MACROS_H
#define __MACROS_H

/** @file */

/** @defgroup Macros Macros
 * @{
 * Useful defined macros
 */

#define BIT(n) (0x01<<(n))

#define TIMER0_IRQ	        0    /**< @brief Timer 0 IRQ line */
#define KBD_IRQ	        	1    /**< @brief KBD IRQ line */
#define PS2_IRQ	        	12    /**< @brief PS/2 IRQ line */

//global constants
#define STATUS_REGISTER 	0x64 /**< @brief KBD Controller port */
#define KBC_CMD_REGISTER	0x64 /**< @brief KBD Command Register */
#define COMMAND_REGISTER	0x60 /**< @brief Command Register */
#define OUTPUT_BUFFER		0x60 /**< @brief Output Buffer */

#define OUTPUT_BUFFER_FULL 	BIT(0)
#define INPUT_BUFFER_FULL	BIT(1)

//KBD #defines
#define KBD_TWO_BYTES		0xE0 /**< @brief Two Byte scancode */
#define KBD_ONE_BYTE		0x00 /**< @brief One byte scancode */
#define KEY_ESC				0x81 /**< @brief Escape scancode */

#define KEY_C				0x2E /**< @brief Copy */
#define KEY_X				0x2D /**< @brief Cut */
#define KEY_V				0x2F /**< @brief Paste */

#define KEY_Z				0x2C /**< @brief Backwards */
#define KEY_Y				0x15 /**< @brief Forward  */

#define KEY_D				0x20 /**< @brief Delete line */

#define KEY_N				0x31 /**< @brief New file */
#define KEY_O				0x18 /**< @brief Open file */
#define KEY_S				0x1F /**< @brief Save */
#define KEY_E				0x12 /**< @brief EXit */

#define KEY_PLUS			0x1A /**< @brief  */
#define KEY_MINUS			0x35 /**< @brief  */
#define KEY_BACKSPACE		0x0E /**< @brief  */
#define KEY_DELETE			0x53 /**< @brief  */
#define KEY_ENTER			0x1C /**< @brief  */
#define KEY_UP				0x48 /**< @brief  */
#define KEY_LEFT			0x4B /**< @brief  */
#define KEY_RIGHT			0x4D /**< @brief  */
#define KEY_DOWN			0x50 /**< @brief  */

//Timer #defines

//VIDEO GRAPHICS #defines
#define GRAPHICS_MODE 		0x105 /**< @brief 1024x768 8 bit color graphics mode */
#define H_RES         	 	1024 /**< @brief height of screen */
#define V_RES		  		768 /**< @brief width of the screen */
#define BITS_PER_PIXEL		8 /**< @brief number of bits per pixel */

#define LINEAR_MODEL_BIT 	14
#define PB2BASE(x) 			(((x) >> 4) & 0x0F000)
#define PB2OFF(x) 			((x) & 0x0FFFF)

#define VBE_BIOS_V_SERV		0x10
#define VBE_MODE_INFO		0x4F01

#define TRANSPARENT			0x88
#define BACKGROUND			0x00 /**< @brief Black */
#define FONT_COLOR			0x0F /**< @brief White */

#define SCREEN_CHARS_HOR	63 /**< @brief How many chars per row */
#define SCREEN_CHARS_VER	54 /**< @brief how many rows */

#define MAX_LINES			54 /**< @brief Maximum lines */

//MOUSE

#define WRITE_BYTE_TO_MOUSE 0xD4
#define MOUSE_Y_SIGNAL		5
#define MOUSE_X_SIGNAL		4
#define MOUSE_MB			BIT(2)
#define MOUSE_RB			BIT(1)
#define MOUSE_LB			BIT(0)
#define ENABLE_STREAM_MODE	0xEA
#define MOUSE_MAX_ATTEMPTS	5
#define MOUSE_ENABLE		0xF4
#define DISABLE_STREAM_MODE	0xF5
#define DELAY_MOUSE			20000 /**< @brief Delay value */
#define ACK					0xFA /**< @brief Ackknowledgement */
#define NACK				0xFE


#endif /**@}*/
