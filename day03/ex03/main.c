#include <avr/io.h>
void	USART_RX_vect(void) __attribute__ ((signal, used, externally_visible));

void	uart_tx(uint8_t character) {
	while (!(UCSR0A & _BV(UDRE0)));					// Wait for the Data Register Empty event.
	UDR0 = character;								// Place character into transmission buffer.
}

void 	USART_RX_vect(void) {
	uint8_t my_byte = UDR0;							// The read byte is available in UDR0.
	if (my_byte >= 'a' && my_byte <= 'z')
		uart_tx(my_byte -  'a' + 'A');
	else if (my_byte >= 'A' && my_byte <= 'Z')
		uart_tx(my_byte - 'A' +  'a');
	else 
		uart_tx(my_byte);
}

void	uart_init(uint32_t baudrate) {
	uint32_t ubbr_value = F_CPU/(16*baudrate);		// witout the -1 because the integer logic truncates instead of rounding
	UBRR0H = (uint8_t) (ubbr_value >> 8);			// Put the upper 8 bit value of UBBR. 
	UBRR0L = (uint8_t) ubbr_value;					// Put the lower 8 bit value of UBBR. 
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);				// Set payload length to 8 bits.
	UCSR0B = _BV(TXEN0) | _BV(RXEN0) | _BV(RXCIE0);	// Enable UART transmission and reception; Enable RX Complete Interrupt.
}

int		main(void) {
	uart_init(115200);
	__asm__ __volatile__ ("sei" ::: "memory");		// Enable interrupts.
	while(1);
}