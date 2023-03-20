#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

volatile uint8_t counter;

void	uart_init(uint32_t baudrate);
void	uart_tx(uint8_t character);
void	uart_putnbr(uint32_t nb);
void	uart_puthex(uint8_t c);
void	uart_putstr(volatile uint8_t* string);

ISR(INT0_vect) {
	static uint8_t falling_edge = 1;
	if (falling_edge){
		counter += 1 ;
		PORTB = counter & 0b00000111;
		PORTB |= counter >> 3 << 4;
		eeprom_write_byte((uint8_t*)0x0000, counter);
		uart_puthex(counter);
	}
	_delay_ms(1);
	falling_edge ^= 1;
	EIFR |= _BV(INT0);                                          // Clear the interrupt flag for any bouncing that happened during the pause.
}

int main(void) {
	uart_init(115200);
	DDRB = _BV(PB0) | _BV(PB1) | _BV(PB2) | _BV(PB4);
	DDRD = 0x00;
	PORTB = 0x00;

	EICRA = _BV(ISC00);                                         // Set External Interrupt Control to React To Any Logical Change mode for INT0.
	EIMSK = _BV(INT0);                                          // Declare INT0 in the External Interrupt Mask Register

	counter = eeprom_read_byte((uint8_t*)0x0000);
	uart_puthex(counter);
	PORTB = counter & 0b00000111;
	PORTB |= counter >> 3 << 4;
	sei();
	while (1);
}


	