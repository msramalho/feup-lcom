#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#define BIT(n) (0x01<<(n))

#define KBD_IRQ	        	1    /**< @brief KBD IRQ line */

#define STATUS_REGISTER 	0x64
#define KBC_CMD_REGISTER	0x64
#define COMMAND_REGISTER	0x60

#define ESC_KEY				0x81

#define OUTPUT_BUFFER		0x60
/*#define INPUT_BUFFER		0x64*/

#define OUTPUT_BUFFER_FULL 	BIT(0)
#define INPUT_BUFFER_FULL	BIT(1)

#define KBD_TWO_BYTES_CODE	0xE0

#define DELAY_US    		20000 /**< @brief ms to wait before sending the command */

#define KBD_LEDS_COMMAND	0xED

#define LED(n)				BIT(n)

#define ACK					0xFA
#define RESEND				0xFE
#define ERROR				0xFC

#define ERROR_PARITY		BIT(7)
#define ERROR_TIMEOUT		BIT(6)

#define MAX_KBC_ATTEMPTS	5

#endif /* _LCOM_I8042_H */
