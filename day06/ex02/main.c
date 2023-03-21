#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void	uart_init(uint32_t baudrate);
void	uart_tx(uint8_t character);
void	uart_putnbr(uint32_t nb);
void	uart_puthex(uint8_t c);
void	uart_putstr(volatile uint8_t* string);

void	set_rgb(uint8_t r, uint8_t g, uint8_t b) {
	OCR0B = r;
	OCR0A = g;
	OCR2B = b;
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

void	init_rgb() {
	DDRD = _BV(PD3) | _BV(PD5) | _BV(PD6);							// Set PD3/OC2B, PD5/OC0B and PD6/OC0A as outputs.

    TCNT0 = 0;														// Initialise TC0 value to 0.
	OCR0A = 0;														// Set OCR0A to 0% of TOP (0xFF)
	OCR0B = 0;														// Set OCR0B to 0% of TOP (0xFF)
	TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);	// Set Compare Output A & B Mode to COM0X1:0 = 10 (Set OC0X at BOTTOM, clear it at Compare); Set TC0 mode to Fast PWM 0xFF (WGM02:0 = 3)
    TCCR0B = _BV(CS00);               								// Set up TC0 pre-scaler to 1 (this starts the timer)

	TCNT2 = 0;														// Initialise TC2 value to 0.
	OCR2B = 0;														// Set OCR2B to 0% of TOP (0xFF)
	TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);					// Set Compare Output B Mode to COM2B1:0 = 10 (Set OC2B at BOTTOM, clear it at Compare); Set TC2 mode to Fast PWM 0xFF (WGM22:0 = 3)
	TCCR2B = _BV(CS20);												// Set up TC2 pre-scaler to 1 (this starts the timer)

}

ISR(TIMER1_OVF_vect) {
	static uint8_t wheel_pos;
	wheel(++wheel_pos);
}

int main(void) {
	uart_init(115200);
	init_rgb();

	TCNT1 = 0; 														// Initialise TC1 value to 0.
	ICR1 = 2940;													// Set ICR1/TOP at 2941 ticks (with a pre-scaler at 64, it represents a 1/85 sec. period).

	TIMSK1 = _BV(TOIE1);											// Enable OVF interrupt
	TCCR1A = _BV(WGM11);											// Set TC1 mode to Fast PWM ICR1 (WGM13:0 = 14)
	TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11) | _BV(CS10);		// Set TC1 mode to Fast PWM ICR1 (WGM13:0 = 14); Set up TC1 pre-scaler to 64 (this starts the timer)

	set_rgb(0xFF, 0, 0);
	sei();
	while (1);
}


	