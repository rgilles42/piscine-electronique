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

t_RGB	leds[6];

void SPI_MasterInit(void) {
	DDR_SPI = _BV(P_MOSI) | _BV(P_SCK) | _BV(P_SS);			// Set MOSI, SCK and SS output.
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);				// Enable SPI, Master, set clock rate fck/16
}

void inline SPI_write(char cData) {
	SPDR = cData;											// Start transmission
	while(!(SPSR & _BV(SPIF)));								// Wait for transmission complete
}

void set_leds(t_RGB* led_array, uint16_t nb_leds, uint8_t brightness)
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

ISR(ADC_vect) {
	if (ADCH >= 0xFF)
		set_leds(leds, 3, 31);
	else if (ADCH >= 170)
		set_leds(&(leds[1]), 3, 31);
	else if (ADCH >= 85)
		set_leds(&(leds[2]), 3, 31);
	else
		set_leds(&(leds[3]), 3, 31);
}

int main(void) {
	uart_init(115200);
	ADMUX = _BV(REFS0) | _BV(ADLAR);																// Set ADC Ref value to AVCC; Set result left alignment for 8-bit reading
	ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);	// Enable ADC; Perform first measure; Enable auto-trigger; Enable measure complete interrupt; Set prescaler to 128
	SPI_MasterInit();

	leds[0].r = 0x03;
	leds[0].g = 0x03;
	leds[0].b = 0x03;

	leds[1].r = 0x03;
	leds[1].g = 0x03;
	leds[1].b = 0x03;

	leds[2].r = 0x03;
	leds[2].g = 0x03;
	leds[2].b = 0x03;

	leds[3].r = 0x00;
	leds[3].g = 0x00;
	leds[3].b = 0x00;

	leds[4].r = 0x00;
	leds[4].g = 0x00;
	leds[4].b = 0x00;

	leds[5].r = 0x00;
	leds[5].g = 0x00;
	leds[5].b = 0x00;
	
	sei(); 
	while (1);
}


	