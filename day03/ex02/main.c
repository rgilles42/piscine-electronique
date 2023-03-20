#include <avr/io.h>

void uart_init(uint32_t baudrate) {
	uint32_t ubbr_value = F_CPU/(16*baudrate);		// witout the -1 because the integer logic truncates instead of rounding
	UBRR0H = (uint8_t) (ubbr_value >> 8);			// Put the upper 8 bit value of UBBR. 
	UBRR0L = (uint8_t) ubbr_value;					// Put the lower 8 bit value of UBBR. 
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);				// Set payload length to 8 bits.
	UCSR0B = _BV(TXEN0) | _BV(RXEN0);				// Enable UART transmission and reception.
}

void uart_tx(uint8_t character) {
	while (!(UCSR0A & _BV(UDRE0)));					// Wait for the Data Register Empty event.
	UDR0 = character;								// Place character into transmission buffer.
}

void uart_rx() {
	while (!(UCSR0A & _BV(RXC0)));					// Wait for the Receive Complete event.
	uart_tx(UDR0);									// The read byte is available in UDR0.
}

int	main(void) {
	uart_init(115200);
	while(1) {
		uart_rx();
	}
}