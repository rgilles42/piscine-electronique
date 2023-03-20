#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

volatile uint8_t*	counter_addr;							// We will use 6 bytes at this address, 0:1 => magic number; 2:5 => counter 1-4
volatile uint8_t	counter_offset;
volatile uint8_t	count;

void	uart_init(uint32_t baudrate);
void	uart_tx(uint8_t character);
void	uart_putnbr(uint32_t nb);
void	uart_puthex(uint8_t c);
void	uart_putstr(volatile uint8_t* string);

void	init_space(volatile uint8_t* offset) {
	if (((uint16_t)eeprom_read_byte(offset) << 8 | (uint16_t)eeprom_read_byte(offset + 1)) != 0xBEEF) {		// If we cannot find our magic number at offset, clear out the space we will be using.
		eeprom_write_byte(offset, 0xBE);
		eeprom_write_byte(offset + 1, 0xEF);
		for (uint8_t i = 2; i < 6; i++) {
			eeprom_write_byte(offset + i, 0x00);
		}
	}

}

ISR(INT0_vect) {
	static uint8_t falling_edge = 1;
	if (falling_edge){
		count += 1 ;
		PORTB = count & 0b00000111;
		PORTB |= count >> 3 << 4;
		eeprom_write_byte(counter_addr + counter_offset, count);
		uart_puthex(count);
	}
	_delay_ms(1);
	falling_edge ^= 1;
	EIFR |= _BV(INT0);                                          // Clear the interrupt flag for any bouncing that happened during the pause.
}

ISR(PCINT2_vect) {
	static uint8_t falling_edge = 1;
	if (falling_edge){
		++counter_offset;
		if (counter_offset >= 6)
			counter_offset = 2;
		count = eeprom_read_byte(counter_addr + counter_offset);
		uart_puthex(count);
		PORTB = count & 0b00000111;
		PORTB |= count >> 3 << 4;
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

	PCICR = _BV(PCIE2);                                         // Enable Pin Change Interrupt for PCINT pin group 2.
	PCMSK2 = _BV(PCINT20);                                      // Enable PCINT20 in the Pin Change Mask Register for PCINT pin group 2

	counter_addr = 0x0000;
	counter_offset = 2;

	init_space(counter_addr);

	count = eeprom_read_byte(counter_addr + counter_offset);
	uart_puthex(count);
	PORTB = count & 0b00000111;
	PORTB |= count >> 3 << 4;
	sei();
	while (1);
}


	