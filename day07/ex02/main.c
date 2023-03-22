#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void	uart_init(uint32_t baudrate);
void	uart_tx(uint8_t character);
void	uart_putnbr(uint32_t nb);
void	uart_puthex(uint8_t c);
void	uart_putstr(volatile uint8_t* string);

ISR(ADC_vect) {
	static uint8_t	count = 0;
	uart_putnbr(ADC);
	if (count != 0)
		uart_putstr(", ");
	else {
		uart_putstr("\r\n");
		_delay_ms(20);
	}
	++count;
	if (count == 3)
		count = 0;
	ADMUX = _BV(REFS0) | count;
	ADCSRA |= _BV(ADSC);
}

int main(void) {
	uart_init(115200);

	ADMUX = _BV(REFS0);																				// Set ADC Ref value to AVCC
	ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);	// Enable ADC; Perform first measure; Enable auto-trigger; Enable measure complete interrupt; Set prescaler to 128

	sei();
	while (1);
}


	