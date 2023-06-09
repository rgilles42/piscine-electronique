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

uint8_t	regs[] = {0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF};

void	exp_receive(uint8_t ack) {
	i2c_start();
	i2c_write(EXP_ADDRESS | I2C_READ);
	i2c_read(ack);
	i2c_stop();
}

void	exp_transmit(uint8_t* payload, uint8_t nb_bytes) {
	i2c_start();
	i2c_write(EXP_ADDRESS | I2C_WRITE);
	for (uint8_t i = 0; i < nb_bytes; ++i)
		i2c_write(payload[i]);
	i2c_stop();
}

void	exp_write_register(uint8_t reg_index) {
	uint8_t	cmd[] = {reg_index, regs[reg_index]};
	exp_transmit(cmd, 2);
}

void	exp_read_register(uint8_t reg_index) {
	exp_transmit(&reg_index, 1);
	exp_receive(I2C_NACK);
	regs[reg_index] = TWDR;
}

int main(void) {
	uint8_t	count = 0;

	uart_init(115200);
	i2c_init(100000);

	regs[EXP_CNF0_REG] &= ~_BV(3) & ~_BV(2) & ~_BV(1);
	exp_write_register(EXP_CNF0_REG);
	while (1) {
		if (count == 8)
			count = 0;
		regs[EXP_OUT0_REG] = ~count << 1;
		exp_write_register(EXP_OUT0_REG);
		exp_read_register(EXP_IN0_REG);
		if (!(regs[EXP_IN0_REG] & _BV(0))) {
			++count;
			_delay_ms(1);
			while(!(regs[EXP_IN0_REG] & _BV(0)))
				exp_read_register(EXP_IN0_REG);
		}
	}
}


	