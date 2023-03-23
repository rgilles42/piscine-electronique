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

void inline SPI_MasterInit(void) {
	DDR_SPI = _BV(P_MOSI) | _BV(P_SCK) | _BV(P_SS);		// Set MOSI, SCK and SS output.
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);				// Enable SPI, Master, set clock rate fck/16
}

void inline SPI_write(char cData) {
	SPDR = cData;											// Start transmission
	while(!(SPSR & _BV(SPIF)));								// Wait for transmission complete
}

void inline set_leds(t_RGB* led_array, uint16_t nb_leds, uint8_t brightness)
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

int main(void) {

	uart_init(115200);	
	SPI_MasterInit();

	uint8_t	count = 1;
	while (1) {
		uint32_t colour = 0x000000;							// We build a uint32 to contain R, G and B in this order because AVR is little-endian
		if (count < 3 || count == 6)
			colour |= 0x080000;								// Turn on Red
		if (!count || (count > 1 && count < 5))
			colour |= 0x000800;								// Turn on Green
		if (!count || count > 3)
			colour |= 0x000008;								// Turn on Blue
		set_leds((t_RGB*)&colour, 1, 31);
		++count;
		if (count == 7)
			count = 0;
		_delay_ms(1000);
	}
}


	