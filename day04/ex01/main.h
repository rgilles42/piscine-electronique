#ifndef _MAIN_H_
# define _MAIN_H_

# include <avr/io.h>

# define F_I2C		100000UL

# define I2C_READ	1
# define I2C_WRITE	0

# define I2C_ACK	1
# define I2C_NACK	0

# define AHT20_ADDRESS			0x38
# define AHT20_INIT_CMD			0xBE
# define AHT20_INIT_PARAM_1		0x08
# define AHT20_INIT_PARAM_2		0x00
# define AHT20_MEASURE_CMD		0xAC
# define AHT20_MEASURE_PARAM_1	0x33
# define AHT20_MEASURE_PARAM_2	0x00
# define AHT20_SOFT_RESET_CMD	0xBA
# define AHT20_STATUS_WORD		0x71

# define AHT20_BUSY	7
# define AHT20_CAL	3

# define AHT20_AFTER_POWER_ON_DELAY	40
# define AHT20_RESET_DELAY			20
# define AHT20_MEASURE_DELAY		80

void i2c_init();
void i2c_start();
void i2c_stop();
void i2c_write( unsigned char data );
void i2c_read( uint8_t ack );

#endif