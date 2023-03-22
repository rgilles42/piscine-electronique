#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void	uart_init(uint32_t baudrate);
void	uart_tx(uint8_t character);
void	uart_putnbr(uint32_t nb);
void	uart_puthex(uint8_t c);
void	uart_putstr(volatile uint8_t* string);

ISR(ADC_vect) {
	uart_puthex(ADCH);
	uart_putstr("\r\n");
	_delay_ms(20);
	ADCSRA |= _BV(ADSC);
}

int main(void) {
	uart_init(115200);

	ADMUX = _BV(REFS0) | _BV(ADLAR);																// Set ADC Ref value to AVCC; Set result left alignment for 8-bit reading
	ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);	// Enable ADC; Perform first measure; Enable auto-trigger; Enable measure complete interrupt; Set prescaler to 128

	sei();
	while (1);
}


	