#include <avr/io.h>
#define F_I2C		100000UL

void	i2c_read(uint8_t ack) {
	TWCR = _BV(TWINT) | (( ack ? 1 : 0) <<TWEA) | _BV(TWEN);	// Send ACK to ask for a byte.
	while (!(TWCR & _BV(TWINT))); 								// Wait for TWINT to set => data has been received.	
}

void	i2c_write(unsigned char data) {
	TWDR = data;												// Load TWI Data Register with Slave Address + Write bit at 0.
	TWCR = _BV(TWINT) | _BV(TWEN);								// Send TWDR content.
	while (!(TWCR & _BV(TWINT)));								// Wait for TWINT to set => Data has been transmitted
}

void	i2c_start(void){
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); 				// Send START conditon.
	while (!(TWCR & _BV(TWINT))); 								// Wait for TWINT to set => START has been transmitted.
}

void	i2c_stop(void){
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO); 				// Send STOP condition.
	TWCR &= ~(_BV(TWEN));										// Disable TWI
}

void	i2c_init() {
	TWBR = (uint8_t)(F_CPU/2UL * F_I2C - 8UL);					//  F_I2C = F_CPU/(16+2*TWBR) <=> TWBR = F_CPU/2*F_I2C - 8
	TWCR = _BV(TWEN);											// Enable TWI operation.
}