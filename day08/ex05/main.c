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
volatile uint8_t	current_primary;
volatile uint8_t	current_led;


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

void wheel(uint8_t pos, uint8_t current_led) {
	pos = 255 - pos;
	if (pos < 85) {
		leds[current_led].r = 255 - pos * 3;
		leds[current_led].g = 0;
		leds[current_led].b = pos * 3;
	} else if (pos < 170) {
		leds[current_led].r = 0;
		leds[current_led].g = (pos - 85) * 3;
		leds[current_led].b = 255 - (pos - 85) * 3;
	} else {
		leds[current_led].r = (pos - 170) * 3;
		leds[current_led].g = 255 - (pos - 170) * 3;
		leds[current_led].b = 0;
	}
}

ISR(ADC_vect) {
	switch (current_primary) {
		case 0:
			leds[current_led].r = ADCH;
			break;
		case 1:
			leds[current_led].g = ADCH;
			break;
		case 2:
			leds[current_led].b = ADCH;
			break;
	}
	set_leds(leds, 3, 1);
}

ISR(INT0_vect) {
    static uint8_t falling_edge = 1;
    if (falling_edge){
        ++current_primary;
		if (current_primary == 3)
			current_primary = 0;
    }
    _delay_ms(1);                                               // Ignore bounce-back.
    falling_edge ^= 1;                                          // Next interrupt will be for rising edge.
    EIFR |= _BV(INT0);                                          // Clear the interrupt flag for any bouncing that happened during the pause.
}

ISR(PCINT2_vect) {
    static uint8_t falling_edge = 1;
    if (falling_edge){
		++current_led;
		if (current_led == 3)
			current_led = 0;
        current_primary = 0;
    }
    _delay_ms(1);                                               // Ignore bounce-back.
    falling_edge ^= 1;                                          // Next interrupt will be for rising edge.
    EIFR |= _BV(INT0);                                          // Clear the interrupt flag for any bouncing that happened during the pause.
}

int main(void) {
	uart_init(115200);
	ADMUX = _BV(REFS0) | _BV(ADLAR);																// Set ADC Ref value to AVCC; Set result left alignment for 8-bit reading
	ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);	// Enable ADC; Perform first measure; Enable auto-trigger; Enable measure complete interrupt; Set prescaler to 128
	EICRA = _BV(ISC00);                                         									// Set External Interrupt Control to React To Any Logical Change mode for INT0.
	EIMSK = _BV(INT0);                                          									// Declare INT0 in the External Interrupt Mask Register
	PCICR = _BV(PCIE2);                                         									// Enable Pin Change Interrupt for PCINT pin group 2.
	PCMSK2 = _BV(PCINT20);																			// Enable PCINT20 in the Pin Change Mask Register for PCINT pin group 2
	SPI_MasterInit();

	leds[0].r = 0x00;
	leds[0].g = 0x00;
	leds[0].b = 0x00;

	leds[1].r = 0x00;
	leds[1].g = 0x00;
	leds[1].b = 0x00;

	leds[2].r = 0x00;
	leds[2].g = 0x00;
	leds[2].b = 0x00;
	
	set_leds(leds, 3, 1);
	sei(); 
	while (1);
}


	