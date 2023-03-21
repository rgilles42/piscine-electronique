#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void	uart_init(uint32_t baudrate);
void	uart_tx(uint8_t character);
void	uart_putnbr(uint32_t nb);
void	uart_puthex(uint8_t c);
void	uart_putstr(volatile uint8_t* string);

volatile uint8_t	is_filled = 0;
volatile uint8_t	current_len = 0;
volatile uint8_t	entered_chars[] = {0, 0, 0, 0, 0, 0, 0};

ISR(USART_RX_vect) {						
	uint8_t my_byte = UDR0;							// The read byte is available in UDR0.
	if (my_byte == 13) {							// If Enter is received.
		uart_tx(my_byte);
		is_filled = 1;
	} else if (my_byte == 127) {					// If DEL is received.
		if (current_len) {
			uart_tx('\b');							// Print non-destructive backspace
			uart_tx(' ');							// Print space over char to erase
			uart_tx('\b');							// Print non-destructive backspace
			--current_len;
		}
	} else if (current_len < 7) {
		uart_tx(my_byte);
		entered_chars[current_len] = my_byte;
		++current_len;
	}
}

uint8_t	invalid_input() {
	if (entered_chars[0] != '#')
		return 1;
	for (uint8_t i = 1; i < 7; i++) {
		if (!(entered_chars[i] >= '0' && entered_chars[i] <= '9') && !(entered_chars[i] >= 'A' && entered_chars[i] <= 'F'))
			return 1;
	}
	return 0;
}

void	set_rgb(uint8_t r, uint8_t g, uint8_t b) {
	OCR0B = r;
	OCR0A = g;
	OCR2B = b;
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

int main(void) {
	uint8_t	cols[] = {0, 0, 0};
	
	init_rgb();
	uart_init(115200);
	while (1) {
		sei();
		uart_putstr("> ");
		while (!is_filled);
		cli();
		uart_putstr("\r\n");
		if (invalid_input())
			uart_putstr("Invalid colour.\r\n");
		else {
			for (uint8_t i = 0; i < 3; ++i) {
				entered_chars[2 * i + 1] -= entered_chars[2 * i + 1] < 'A' ? '0' : 'A' - 10;
				cols[i] = entered_chars[2 * i + 1] << 4;
				entered_chars[2 * i + 2] -= entered_chars[2 * i + 2] < 'A' ? '0' : 'A' - 10;
				cols[i] |= entered_chars[2 * i + 2];
			}
			set_rgb(cols[0], cols[1], cols[2]);
		}
		is_filled = 0;
		current_len = 0;
		for (uint8_t i = 0; i < 6; ++i)
			entered_chars[i] = 0;
	}
}


	