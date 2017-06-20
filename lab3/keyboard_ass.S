#include "i8042.h"

.global _kbd_int_handler_ass

_kbd_int_handler_ass:
		/*xor	%al, %al*/
		inb $(OUTPUT_BUFFER), %al
		/*movb %al, _scan*/
		ret

/*LOOP:	inb	$STATUS_REGISTER
		movb %al, %bl
		and OUTPUT_BUFFER_FULL, %al
		jnz L1
		jmp LOOP
L1:		inb $OUTPUT_BUFFER, %al
		and ERROR_PARITY | ERROR_TIMEOUT, %bl
		jz end

		movb 0xFF, %al
end:
		movb %al, _scan
		ret*/
