#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include "main.h"


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

void	print_hex_value(uint8_t c) {
	uint8_t half_byte;
	for (int8_t hb_index = 4; hb_index >= 0; hb_index -= 4) {
		half_byte = (c >> hb_index) & 0x0F;
		if (half_byte > 9)
			uart_tx(half_byte - 10 + 'A');
		else
			uart_tx(half_byte + '0');
	}
	uart_tx(' ');
}
//-----------------------------------------------------------------------------------

void	i2c_read(uint8_t ack) {
	TWCR = _BV(TWINT) | (( ack ? 1 : 0) <<TWEA) | _BV(TWEN);	// Send ACK to ask for a byte.
	while (!(TWCR & _BV(TWINT))); 				// Wait for TWINT to set => data has been received.
	print_hex_value(TWDR);	
}

void	i2c_write(unsigned char data) {
	TWDR = data;								// Load TWI Data Register with Slave Address + Write bit at 0.
	TWCR = _BV(TWINT) | _BV(TWEN);				// Send TWDR content.
	while (!(TWCR & _BV(TWINT)));				// Wait for TWINT to set => Data has been transmitted
}

void	i2c_start(void){
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); // Send START conditon.
	while (!(TWCR & _BV(TWINT))); 				// Wait for TWINT to set => START has been transmitted.
}

void	i2c_stop(void){
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO); // Send STOP condition.
	TWCR &= ~(_BV(TWEN));						// Disable TWI
}

void	i2c_init() {
	TWBR = (uint8_t)(F_CPU/2UL * F_I2C - 8UL);	//  F_I2C = F_CPU/(16+2*TWBR) <=> TWBR = F_CPU/2*F_I2C - 8
	TWCR = _BV(TWEN);							// Enable TWI operation.
}

//-----------------------------------------------------------------------------------

void	aht20_send_cmd( uint8_t cmd ) {
	i2c_start();
	i2c_write(AHT20_ADDRESS << 1 | I2C_WRITE);
	i2c_write(cmd);
	i2c_stop();
}

void	aht20_send_cmd_params( uint8_t cmd, uint8_t param1, uint8_t param2 ) {
	i2c_start();
	i2c_write(AHT20_ADDRESS << 1 | I2C_WRITE);
	i2c_write(cmd);
	i2c_write(param1);
	i2c_write(param2);
	i2c_stop();
}

void	aht20_read_one() {
	i2c_start();
	i2c_write(AHT20_ADDRESS << 1 | I2C_READ);
	i2c_read(I2C_NACK);
	i2c_stop();
}

void	aht20_read_many(uint8_t nb_bytes) {
	i2c_start();
	i2c_write(AHT20_ADDRESS << 1 | I2C_READ);
	i2c_read(I2C_ACK);
	if( !(TWDR & _BV(AHT20_BUSY)) ) {
		for( uint8_t i = 0; i < nb_bytes - 1; i++ )
			i2c_read( i == nb_bytes - 2 ? I2C_NACK : I2C_ACK );
		i2c_stop();
	}
}

void	aht20_init() {
	_delay_ms(AHT20_AFTER_POWER_ON_DELAY);
	aht20_send_cmd(AHT20_STATUS_WORD);
	aht20_read_one();
	if( !(TWDR & _BV(AHT20_CAL)) ) {
		aht20_send_cmd_params(AHT20_INIT_CMD, AHT20_INIT_PARAM_1, AHT20_INIT_PARAM_2);
		_delay_ms(AHT20_AFTER_POWER_ON_DELAY);
	}
}


int		main(void) {
	uart_init(115200);
	i2c_init(100000);
	aht20_init();
	while (1) {
		uart_tx('\r');
		uart_tx('\n');

		aht20_send_cmd_params(AHT20_MEASURE_CMD, AHT20_MEASURE_PARAM_1, AHT20_MEASURE_PARAM_2);
		_delay_ms(AHT20_MEASURE_DELAY);

		aht20_read_many(7);
	}
	return(0);
}
