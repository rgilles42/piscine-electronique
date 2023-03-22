#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void	uart_init(uint32_t baudrate);
void	uart_tx(uint8_t character);
void	uart_putnbr(uint32_t nb);
void	uart_puthex(uint8_t c);
void	uart_putstr(volatile uint8_t* string);

void	set_rgb(uint8_t r, uint8_t g, uint8_t b) {
	OCR0B = 0xFF - r;
	OCR0A = 0xFF - g;
	OCR2B = 0xFF - b;
}

void wheel(uint8_t pos) {
	pos = 255 - pos;
	if (pos < 85) {
		set_rgb(255 - pos * 3, 0, pos * 3);
	} else if (pos < 170) {
		pos = pos - 85;
		set_rgb(0, pos * 3, 255 - pos * 3);
	} else {
		pos = pos - 170;
		set_rgb(pos * 3, 255 - pos * 3, 0);
	}
}

ISR(ADC_vect) {
	wheel(ADCH);
	PORTB = (ADCH >= 63) | (ADCH >= 127) << 1 | (ADCH >= 191) << 2 | (ADCH >= 255) << 4;
}

void	init_rgb() {
	DDRD = _BV(PD3) | _BV(PD5) | _BV(PD6);														// Set PD3/OC2B, PD5/OC0B and PD6/OC0A as outputs.

    TCNT0 = 0;																					// Initialise TC0 value to 0.
	OCR0A = 0xFF;																				// Set OCR0A to 100% of TOP (0xFF) => duty cycle of 0% in inverted Compare Output mode (thus avoiding the spike)
	OCR0B = 0xFF;																				// Set OCR0B to 100% of TOP (0xFF) => duty cycle of 0% in inverted Compare Output mode (thus avoiding the spike)
	TCCR0A = _BV(COM0A1) | _BV(COM0A0) | _BV(COM0B1) | _BV(COM0B0) | _BV(WGM01) | _BV(WGM00);	// Set Compare Output X Mode to COM0X1:0 = 11 (Clear OC0X at BOTTOM, set at Compare); Set TC0 mode to Fast PWM 0xFF (WGM02:0 = 3)
    TCCR0B = _BV(CS00);               															// Set up TC0 pre-scaler to 1 (this starts the timer)

	TCNT2 = 0;																					// Initialise TC2 value to 0.
	OCR2B = 0xFF;																				// Set OCR2B to 0% of TOP (0xFF)
	TCCR2A = _BV(COM2B1) | _BV(COM2B0) | _BV(WGM21) | _BV(WGM20);								// Set Compare Output B Mode to COM2B1:0 = 11 (Clear OC2B at BOTTOM, set at Compare); Set TC2 mode to Fast PWM 0xFF (WGM22:0 = 3)
	TCCR2B = _BV(CS20);																			// Set up TC2 pre-scaler to 1 (this starts the timer)
}

int main(void) {
	uart_init(115200);
	init_rgb();
	DDRB = _BV(PB0) | _BV(PB1) | _BV(PB2) | _BV(PB4);

	ADMUX = _BV(REFS0) | _BV(ADLAR);																// Set ADC Ref value to AVCC; Set result left alignment for 8-bit reading
	ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);	// Enable ADC; Perform first measure; Enable auto-trigger; Enable measure complete interrupt; Set prescaler to 128

	sei();
	while (1);
}


	