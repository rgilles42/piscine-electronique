#include <avr/io.h>

void mydelay(uint16_t millis) {
	uint32_t nb_loops = F_CPU/1000 * millis / 9;	// The delay loop takes 9 MCU cycles with the -Os flag on my home computer toolchain.
    while (--nb_loops) 
        __asm__ __volatile__("nop");
}

void uart_init(uint32_t baudrate) {
	uint32_t ubbr_value = F_CPU/(16*baudrate);		// witout the -1 because the integer logic truncates instead of rounding
	UBRR0H = (uint8_t) (ubbr_value >> 8);			// Put the upper 8 bit value of UBBR. 
	UBRR0L = (uint8_t) ubbr_value;					// Put the lower 8 bit value of UBBR. 
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);				// Set payload length to 8 bits.
	UCSR0B = _BV(TXEN0);							// Enable UART transmission.
}

void uart_tx(uint8_t character) {
	while (!(UCSR0A & (1<<UDRE0)));					// Wait for the transmission buffer to be cleared.
	UDR0 = character;								// Place character into transmission buffer.
}

int	main(void) {
	uart_init(115200);
	while(1) {
		uart_tx('z');
		mydelay(1000);
	}
}