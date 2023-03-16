#include <avr/io.h>

void	TIMER1_OVF_vect(void) __attribute__ ((signal, used, externally_visible));

void	uart_init(uint32_t baudrate) {
	uint32_t ubbr_value = F_CPU/(16*baudrate);					// witout the -1 because the integer logic truncates instead of rounding
	UBRR0H = (uint8_t) (ubbr_value >> 8);						// Put the upper 8 bit value of UBBR. 
	UBRR0L = (uint8_t) ubbr_value;								// Put the lower 8 bit value of UBBR. 
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);							// Set payload length to 8 bits.
	UCSR0B = _BV(TXEN0);										// Enable UART transmission.
}

void	uart_printstr(uint8_t* string) {
	while (*string) {
		while (!(UCSR0A & _BV(UDRE0)));							// Wait for the transmission buffer to be cleared.
		UDR0 = *string;											// Place character into transmission buffer.
		++string;
	}
	while (!(UCSR0A & _BV(UDRE0)));								// Send '\0'
	UDR0 = *string;
}

void	TIMER1_OVF_vect(void) {									// We know that our overflow interrupt will be triggered each millisecond
	uart_printstr("Hello World!");
}

int		main(void) {
	TIMSK1 = _BV(TOIE1);										// Trigger interrupt at each overflow of TC1 (each millisecond)
	TCNT1 = 0;													// Clear TCNT1
	OCR1A = 31249;												// Set OCR1A/TOP at 31250 ticks (with prescaler at 1024, it represents 2sec).
	TCCR1A = _BV(WGM11) | _BV(WGM10);							// Set TC1 mode to Fast PWM OCR1A (WGM13:0 = 15)
	TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS12) | _BV(CS10);	// Set TC1 mode to Fast PWM OCR1A (WGM13:0 = 15); Set up TC1 pre-scaler to 1024 (this starts the timer)
	uart_init(115200);
	__asm__ __volatile__ ("sei" ::: "memory");					// Enable interrupts.
	while(1);
}