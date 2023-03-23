#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void	uart_init(uint32_t baudrate);
void	uart_tx(uint8_t character);
void	uart_putnbr(uint32_t nb);
void	uart_puthex(uint8_t c);
void	uart_putstr(volatile uint8_t* string);

#define DDR_SPI	DDRB
#define P_MOSI	PB3
#define P_SCK	PB5
#define P_SS	PB2

typedef	struct	s_RGB {										// APA102 parses colours in BGR format.
	uint8_t b;
	uint8_t g;
	uint8_t r;
}				t_RGB;

volatile t_RGB		leds[3];
volatile uint8_t	is_filled = 0;
volatile uint8_t	current_len = 0;
volatile uint8_t	entered_chars[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void SPI_MasterInit(void) {
	DDR_SPI = _BV(P_MOSI) | _BV(P_SCK) | _BV(P_SS);			// Set MOSI, SCK and SS output.
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);				// Enable SPI, Master, set clock rate fck/16
}

void inline SPI_write(char cData) {
	SPDR = cData;											// Start transmission
	while(!(SPSR & _BV(SPIF)));								// Wait for transmission complete
}

void set_leds(volatile t_RGB* led_array, uint16_t nb_leds, uint8_t brightness)
{
	uint8_t*	colours = (uint8_t*)led_array;
 
	SPI_write(0x00);										// Start Frame 
	SPI_write(0x00);
	SPI_write(0x00);
	SPI_write(0x00);
 
	for (uint16_t i = 0; i < nb_leds; ++i)
	{
		SPI_write(0xe0 | brightness);						// 3 * '1´ bits + brightness on 5 bits. 
		SPI_write(colours[3 * i]);							// Blue value (8 bits)
		SPI_write(colours[3 * i + 1]);						// Green
		SPI_write(colours[3 * i + 2]);						// Red
	}
	
	for (uint16_t i = 0; i < nb_leds; i += 16)				// End frame: placeholder data sent to keep the SPI clock running to account for delay in LED-to-LED data propagation.
		SPI_write(0x00);									// Each LED data propagation stops the data throughput for 1/2 a clock cycle.
}															// A transfer of 1 additionnal byte will keep the clock running for 8 cycles, thus
															// 	allowing completion of the data transfer to up to 15 other LEDs in the array.

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
	} else if (current_len < 12) {
		uart_tx(my_byte);
		entered_chars[current_len] = my_byte;
		++current_len;
	}
}

uint8_t	invalid_input() {
	char*	magic = "#FULLRAINBOW";
	if (current_len < 9)
		return 1;
	if (entered_chars[0] != '#')
		return 1;
	if (entered_chars[2] == 'U') {
		if (current_len < 12)
			return 1;
		for (uint8_t i = 1; i < 12; i++) {
			if (entered_chars[i] != magic[i])
				return 1;
		}
	}
	else {
		for (uint8_t i = 1; i < 7; i++) {
			if (!(entered_chars[i] >= '0' && entered_chars[i] <= '9') && !(entered_chars[i] >= 'A' && entered_chars[i] <= 'F'))
				return 1;
		}
		if (entered_chars[7] != 'D' || entered_chars[8] < '6' || entered_chars[8] > '8')
			return 1;
	}
	return 0;
}

void wheel(uint8_t pos) {
	pos = 255 - pos;
	for (uint8_t i = 0; i < 3; ++i) {
		if (pos < 85) {
			leds[i].r = 255 - pos * 3;
			leds[i].g = 0;
			leds[i].b = pos * 3;
		} else if (pos < 170) {
			leds[i].r = 0;
			leds[i].g = (pos - 85) * 3;
			leds[i].b = 255 - (pos - 85) * 3;
		} else {
			leds[i].r = (pos - 170) * 3;
			leds[i].g = 255 - (pos - 170) * 3;
			leds[i].b = 0;
		}
	}
}

ISR(TIMER1_OVF_vect) {
	static uint8_t wheel_pos;
	wheel(++wheel_pos);
	set_leds(leds, 3, 31);
}

int main(void) {
	uint8_t	led_select;

	leds[0].r = 0x00;
	leds[0].g = 0x00;
	leds[0].b = 0x00;
	leds[1].r = 0x00;
	leds[1].g = 0x00;
	leds[1].b = 0x00;
	leds[2].r = 0x00;
	leds[2].g = 0x00;
	leds[2].b = 0x00;

	TCNT1 = 0; 														// Initialise TC1 value to 0.
	ICR1 = 2940;													// Set ICR1/TOP at 2941 ticks (with a pre-scaler at 64, it represents a 1/85 sec. period).

	TIMSK1 = _BV(TOIE1);											// Enable OVF interrupt
	TCCR1A = _BV(WGM11);											// Set TC1 mode to Fast PWM ICR1 (WGM13:0 = 14)
	TCCR1B = _BV(WGM13) | _BV(WGM12);								// Set TC1 mode to Fast PWM ICR1 (WGM13:0 = 14);
	
	uart_init(115200);
	SPI_MasterInit();
	set_leds(leds, 3, 31);
	while (1) {
		sei();
		uart_putstr("> ");
		while (!is_filled);
		cli();
		uart_putstr("\r\n");
		if (invalid_input())
			uart_putstr("Invalid colour.\r\n");
		else {
			if (entered_chars[2] == 'U') {
				TCCR1B |= _BV(CS11) | _BV(CS10);					// Set up TC1 pre-scaler to 64 (this starts the timer)
			} else {
				TCCR1B &= ~(_BV(CS11) | _BV(CS10));					// Stop the timer
				led_select = entered_chars[8] - '0' - 6;
				entered_chars[1] -= entered_chars[1] < 'A' ? '0' : 'A' - 10;
				entered_chars[2] -= entered_chars[2] < 'A' ? '0' : 'A' - 10;
				leds[led_select].r = entered_chars[1] << 4 | entered_chars[2];
				entered_chars[3] -= entered_chars[3] < 'A' ? '0' : 'A' - 10;
				entered_chars[4] -= entered_chars[4] < 'A' ? '0' : 'A' - 10;
				leds[led_select].g = entered_chars[3] << 4 | entered_chars[4];
				entered_chars[5] -= entered_chars[5] < 'A' ? '0' : 'A' - 10;
				entered_chars[6] -= entered_chars[6] < 'A' ? '0' : 'A' - 10;
				leds[led_select].b = entered_chars[5] << 4 | entered_chars[6];
				set_leds(leds, 3, 31);
			}
		}
		is_filled = 0;
		current_len = 0;
		for (uint8_t i = 0; i < 6; ++i)
			entered_chars[i] = 0;
	}
}


	