#include <avr/io.h>

void	uart_init(uint32_t baudrate) {
	uint32_t ubbr_value = F_CPU/(16*baudrate);	// witout the -1 because the integer logic truncates instead of rounding
	UBRR0H = (uint8_t) (ubbr_value >> 8);		// Put the upper 8 bit value of UBBR. 
	UBRR0L = (uint8_t) ubbr_value;				// Put the lower 8 bit value of UBBR. 
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);			// Set payload length to 8 bits.
	UCSR0B = _BV(TXEN0);						// Enable UART transmission.
}

void	uart_tx(uint8_t character) {
	while (!(UCSR0A & _BV(UDRE0)));				// Wait for the Data Register Empty event.
	UDR0 = character;							// Place character into transmission buffer.
}

void	uart_putnbr(uint32_t nb) {
	if (nb >= 10) {
		uart_putnbr(nb / 10);
		uart_putnbr(nb % 10);
	}
	else {
		uart_tx(nb + '0');
	}
}

void	uart_puthex(uint8_t c) {
	uint8_t half_byte;
	for (int8_t hb_index = 4; hb_index >= 0; hb_index -= 4) {
		half_byte = (c >> hb_index) & 0x0F;
		if (half_byte > 9)
			uart_tx(half_byte - 10 + 'A');
		else
			uart_tx(half_byte + '0');
	}
	uart_tx(' ');
}

void	uart_putstr(volatile uint8_t* string) {
	while (*string) {
		uart_tx(*string);
		++string;
	}
	uart_tx(*string);
}