#include <avr/io.h>
#include <util/twi.h>

void	uart_init(uint32_t baudrate) {
	uint32_t ubbr_value = F_CPU/(16*baudrate);	// witout the -1 because the integer logic truncates instead of rounding
	UBRR0H = (uint8_t) (ubbr_value >> 8);		// Put the upper 8 bit value of UBBR. 
	UBRR0L = (uint8_t) ubbr_value;				// Put the lower 8 bit value of UBBR. 
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);			// Set payload length to 8 bits.
	UCSR0B = _BV(TXEN0);						// Enable UART transmission.
}

void	uart_tx(uint8_t character) {
	while (!(UCSR0A & _BV(UDRE0)));				// Wait for the Data Register Empty event.
	UDR0 = character;							// Place character into transmission buffer.
}

void	print_hex_value(uint8_t nb) {
	uint8_t half_byte;
	for (int8_t hb_index = 4; hb_index >= 0; hb_index -= 4) {
		half_byte = (nb >> hb_index) & 0x0F;
		if (half_byte > 9)
			uart_tx(half_byte + 'A');
		else
			uart_tx(half_byte + '0');
	}
	uart_tx(' ');
}

void	i2c_init(uint32_t f_i2c) {
	TWBR = (uint8_t)(F_CPU/2UL * f_i2c - 8UL);	//  F_I2C = F_CPU/(16+2*TWBR) <=> TWBR = F_CPU/2*F_I2C - 8
	TWCR = _BV(TWEN);							// Enable TWI operation.
}

int		i2c_start(void){
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); // Send START conditon.
	while (!(TWCR & _BV(TWINT))); 				// Wait for TWINT to set => START has been transmitted.
	print_hex_value(TWSR);
	if ((TWSR & 0xF8) != 0x08)					// Check if TWSR status register shows START has been transmitted. 
		return(-1);
	TWDR = 0x38 << 1;							// Load TWI Data Register with Slave Address + Write bit at 0.
	TWCR = _BV(TWINT) | _BV(TWEN);				// Send TWDR content.
	while (!(TWCR & _BV(TWINT)));				// Wait for TWINT to set => Data has been transmitted
	print_hex_value(TWSR);
	if ((TWSR & 0xF8) != 0x18)					// Check if TWSR status register shows SLA+W has been transmitted and ACK has been received. 
		return(-2);
	return(0);
}

void	i2c_stop(void){
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO); // Send STOP condition.
	TWCR &= ~(_BV(TWEN));						// Disable TWI
}

int		main(void) {
	uart_init(115200);
	i2c_init(100000);
	if (!i2c_start())
		i2c_stop();
	return(0);
}