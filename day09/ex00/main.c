#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define I2C_READ	1
#define I2C_WRITE	0
#define I2C_ACK		1
#define I2C_NACK	0

#define EXP_ADDRESS		0x40
#define EXP_IN0_REG		0x00
#define EXP_IN1_REG		0x01
#define EXP_OUT0_REG	0x02
#define EXP_OUT1_REG	0x03
#define EXP_PI0_REG		0x04
#define EXP_PI1_REG		0x05
#define EXP_CNF0_REG	0x06
#define EXP_CNF1_REG	0x07

void	uart_init(uint32_t baudrate);
void	uart_tx(uint8_t character);
void	uart_putnbr(uint32_t nb);
void	uart_puthex(uint8_t c);
void	uart_putstr(volatile uint8_t* string);

void	i2c_read(uint8_t ack);
void	i2c_write(unsigned char data);
void	i2c_start(void);
void	i2c_stop(void);
void	i2c_init();

void	exp_transmit_cmd(uint8_t* cmd, uint8_t nb_bytes) {
	i2c_start();
	i2c_write(EXP_ADDRESS | I2C_WRITE);
	for (uint8_t i = 0; i < nb_bytes; ++i)
		i2c_write(cmd[i]);
	i2c_stop();
}

void	exp_receive(uint8_t ack) {
	i2c_start();
	i2c_write(EXP_ADDRESS | I2C_READ);
	i2c_read(ack);								// read data is in TWDR
	i2c_stop();
}

int main(void) {
	uart_init(115200);
	i2c_init(100000);

	uint8_t	enable_o0_3[] = {EXP_CNF0_REG, ~_BV(3)};
	uint8_t	set_o0_3[] = {EXP_OUT0_REG, ~_BV(3)};
	uint8_t	clear_o0_3[] = {EXP_OUT0_REG, 0xFF};

	exp_transmit_cmd(enable_o0_3, 2);

	while (1) {
		exp_transmit_cmd(set_o0_3, 2);
		_delay_ms(100);
		exp_transmit_cmd(clear_o0_3, 2);
		_delay_ms(900);
	}
}


	