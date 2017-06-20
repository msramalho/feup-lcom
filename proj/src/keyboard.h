#ifndef __KEYBOARD_H
#define __KEYBOARD_H


/** @defgroup keyboard keyboard
 * @brief This is the keyboard module, to handle the processing of scancodes, makecodes, keyboard interrupts
 * Highly reusable and scalable module in other Minix projects involving keyboard
 * @{
 * Functions for the keyboard interface
 */

/**
 * @brief kbdAction enum of various available actions triggered by keyboard input
 *
 * CTRL		0X1D (0x9D)
 * ALT		0x38 (0xB8)
 * SHIFT	0X2A (0xAA) 0X36 (0xB6)
 * ALTGR	[0xE0 + 0x38] ([0xE0 + 0xB8])
 * DEL		[0xE0 + 0x53] ([0xE0 + 0xD3])
 * Exemplos:
 * 0x2A - 0x1E - OX9E - 0xAA => Ctrl + 'a' = 'A'
 * 0x2A - 0x1E - OX9E - 0xAA => Ctrl + 'a' = 'A'
 */
typedef enum kbdAction {
	NOTHING,
	WRITE_CHAR,
	COPY,
	PASTE,
	CUT,
	PARAGRAPH,
	BACKSPACE,
	DELETE,
	DELETE_LINE,
	SAVE,
	STOP,
	OPEN_FILE,
	GO_BACK,
	GO_FORWARD,
	MOVE_CURSOR,
	NEW_FILE,
	BACKGROUND_NEXT,
	BACKGROUND_PREV,
	FONT_COLOR_NEXT,
	FONT_COLOR_PREV
} KbdAction;

/**
 * @brief special_key_t byte containing the currently pressed buttons
 *
 * BIT 0 - 1 if ALT is being pressed
 * BIT 1 - 1 if CTRL is being pressed
 * BIT 2 - 1 if SHIFT (LEFT) is being pressed
 * BIT 3 - 1 if SHIFT (RIGHT) is being pressed
 * BIT 4 - 1 if ALTGR is being pressed
 * BIT 5 - 1 if CAPSLOCK is being pressed
 * BIT 6 - 1 if NUMLOCK is being pressed
 * BIT 7 - NOTHING
 * Bit 8 - NOTHING
 */
typedef struct {
	char special_mask; /**@brief Byte holding all the information*/
} special_key_t;

/**
 * @brief code_t struct that holds a unique pair of two bytes (0x00 + makecode OR 0xE0 + makecode)that identify a keyboard key
 */
typedef struct {
	unsigned long byte1;	/**@brief byte 1 is either 0x00 or 0xE0*/
	unsigned long byte2;	/**@brief byte 2 holds the keycode*/
} code_t;

/*
 * @brief key a struct that holds all the information of a keyboard key
 * hexcode, lowercase, uppercase and altgr values of any key that is translated to text.
 */
typedef struct {
	unsigned long hex;	/**@brief hex value of keyboard key, the makecode*/
	char lower; 		/**@brief lowercase value of keyboard key, '\0' if not applicable*/
	char upper; 		/**@brief uppercase value of keyboard key, '\0' if not applicable*/
	char altgr; 		/**@brief altgr value of keyboard key, '\0' if not applicable*/
} key;

/**
 * @brief kbd_tupple a struct holding information relevant to processing the keyboard actions
 */
typedef struct {
	code_t * code;				/**@brief the last valid key clicked*/
	KbdAction * action;			/**@brief the action associated with code_t code variable*/
	key * k;					/**@brief the key to translate the key, if necessary, into its lower, upper or altgr value*/
	special_key_t * special;	/**@brief information about Shift, Ctrl, Altgr, ... @see special_key_t*/
} kbd_tupple;

/**
 * @brief Allocates memory for a new keyboardAction structure
 * @param val The value of the action
 *
 * @return The pointer to new allocated memory
 */
KbdAction * new_kbd_action(KbdAction val);

/**
 * @brief Allocates memory for a new special_key_t
 * @return The pointer to the new allocated special key
 */
special_key_t * new_special_key();

/**
 * @brief Receives a scancode and updates a special_key byte to either
 * add or remove set bits according to the pressed keys
 * @param s The special key to be updated
 * @param scancode The new updated key
 * @param double_key If the scancode is 2 bytes
 */
void update_special_key(special_key_t * s, unsigned long scancode,
		int double_key);

/**
 * @brief Check if special key is CTRL
 * @param s The special key structure we want to check
 *
 * @return 1 if true, 0 on false
 */
int isCTRL(special_key_t * s);

/**
 * @brief Check if special key is ALT
 * @param s The special key structure we want to check
 *
 * @return 1 if true, 0 on false
 */
int isALT(special_key_t * s);

/**
 * @brief Check if special key is SHIFT
 * @param s The special key structure we want to check
 *
 * @return 1 if true, 0 on false
 */
int isSHIFT(special_key_t * s);

/**
 * @brief Check if special key is ALTGR
 * @param s The special key structure we want to check
 *
 * @return 1 if true, 0 on false
 */
int isALTGR(special_key_t * s);

/**
 * @brief Check if special key is CAPSLOCK
 * @param s The special key structure we want to check
 *
 * @return 1 if true, 0 on false
 */
int isCAPSLOCK(special_key_t * s);

/**
 * @brief Check if special key is NUMLOCK
 * @param s The special key structure we want to check
 *
 * @return 1 if true, 0 on false
 */
int isNUMLOCK(special_key_t * s);

/**
 * @brief Allocates the memory for a new code struture
 * @param b1 First byte
 * @param b2 Second byte
 *
 * @return Pointer to the new created structure code
 */
code_t * new_code(unsigned long b1, unsigned long b2);

/**
 * @brief Gets the letter from the key structure
 * @param c The code from which we will return the letter
 *
 * @return Pointer to the key
 */
key * getLETTER(code_t * c);

/**
 * @brief Allocates the memory for a new kbd_tupple struture
 * @param c c Pointer to the code
 * @param ka Pointer to the keyboard action
 * @param k Pointer to the key
 * @param s Pointer to the special key
 *
 * @return Pointer to the new created structure kbd_tupple
 */
kbd_tupple * new_kbd_tupple(code_t * c, KbdAction * ka, key * k,
		special_key_t * s);

/**
 * @brief Gets the Kbdaction
 * @param s Pointer to the special key associated with the Kbdaction
 * @param c Pointer to the code associated with the Kbdaction
 * @param k Pointer to the key associated with the Kbdaction
 *
 * @return Pointer to the KbdAction
 */
KbdAction * get_action(special_key_t * s, code_t * c, key * * k);

/**
 * @brief Reads information from kbd controller
 *
 * @return NULL if the current input is a 0xE0, and returns a code when it is not.
 *
 */
kbd_tupple * kbd_int_handler();

/**
 * @brief Subscribes and enables KBD interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int kbd_subscribe_int();

/**
 * @brief Unsubscribes KBD interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_unsubscribe_int();

#endif /**@}*/
