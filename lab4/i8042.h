#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#define BIT(n) (0x01<<(n))

#define PS2_IRQ	        	12    /**< @brief PS/2 IRQ line */

#define STATUS_REGISTER 	0x64
#define KBC_CMD_REGISTER	0x64
#define COMMAND_REGISTER	0x60

#define OUTPUT_BUFFER		0x60
/*#define INPUT_BUFFER		0x64*/

#define OUTPUT_BUFFER_FULL 	BIT(0)
#define INPUT_BUFFER_FULL	BIT(1)


#define DELAY_US    		20000

#define ACK					0xFA
#define NACK				0xFE
#define ERROR				0xFC

/*#define ERROR_PARITY		BIT(7)*/
/*#define ERROR_TIMEOUT		BIT(6)*/

#define STATUS_REQUEST		0xE9
#define WRITE_BYTE_TO_MOUSE	0xD4
#define ENABLE_STREAM_MODE	0xEA
#define DISABLE_STREAM_MODE	0xF5
#define MOUSE_ENABLE		0xF4

#define MOUSE_MAX_ATTEMPTS		5


#endif
