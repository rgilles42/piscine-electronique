#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void	uart_init(uint32_t baudrate);
void	uart_tx(uint8_t character);
void	uart_putnbr(uint32_t nb);
void	uart_puthex(uint8_t c);
void	uart_putstr(volatile uint8_t* string);

ISR(TIMER1_OVF_vect) {
	switch (PORTD) {
		case _BV(PD5) :
			PORTD |= _BV(PD6);
			break;
		case _BV(PD5) | _BV(PD6) :
			PORTD &= ~_BV(PD5);
			break;
		case _BV(PD6) :
			PORTD |= _BV(PD3);
			break;
		case _BV(PD6) | _BV(PD3) :
			PORTD &= ~_BV(PD6);
			break;
		case _BV(PD3) :
			PORTD |= _BV(PD5);
			break;
		case _BV(PD3) | _BV(PD5) :
			PORTD &= ~_BV(PD3);
			break;
	}
}

int main(void) {
	uart_init(115200);
	DDRD = _BV(PD3) | _BV(PD5) | _BV(PD6);


	TCNT1 = 0; 													// Initialise TC1 value to 0.
	ICR1 = 31250;												// Set ICR1/TOP at 31250 ticks (with a pre-scaler at 256, it represents a 0.5s period).
	
	TIMSK1 = _BV(TOIE1);										// Enable OVF interrupt
	TCCR1A |= _BV(WGM11);										// Set TC1 mode to Fast PWM ICR1 (WGM13:0 = 14)
	TCCR1B |= _BV(WGM13) | _BV(WGM12) | _BV(CS12);	// Set TC1 mode to Fast PWM ICR1 (WGM13:0 = 14); Set up TC1 pre-scaler to 256 (this starts the timer)
	PORTD = _BV(PD5);
	sei();
	while (1);
}


	