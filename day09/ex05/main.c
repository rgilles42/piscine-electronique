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

#define	disp_d1			0x04
#define	disp_d2			0x05
#define	disp_d3			0x06
#define	disp_d4			0x07
#define	disp_a			0x00
#define	disp_b			0x01
#define	disp_c			0x02
#define	disp_d			0x03
#define	disp_e			0x04
#define	disp_f			0x05
#define	disp_g			0x06
#define	disp_dot		0x07

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

void	exp_write_register(uint8_t reg_index, uint8_t update_both) {
	uint8_t	cmd[3] = {reg_index, regs[reg_index], regs[reg_index % 2 == 0 ? ++reg_index: --reg_index]};
	exp_transmit(cmd, update_both ? 3 : 2);
}

void	exp_read_register(uint8_t reg_index) {
	exp_transmit(&reg_index, 1);
	exp_receive(I2C_NACK);
	regs[reg_index] = TWDR;
}

void	disp_init() {
	regs[EXP_CNF0_REG] &= ~_BV(disp_d1) & ~_BV(disp_d2) & ~_BV(disp_d3) & ~_BV(disp_d4);
	regs[EXP_CNF1_REG] = 0x00;
	exp_write_register(EXP_CNF0_REG, 1);
}

void	disp_set(uint8_t digit, uint8_t segments) {
	regs[EXP_OUT0_REG] |= 0xF0;
	regs[EXP_OUT1_REG] = 0x00;
	exp_write_register(EXP_OUT0_REG, 1);
	regs[EXP_OUT0_REG] &= ~_BV(digit);
	regs[EXP_OUT1_REG] = segments;
	exp_write_register(EXP_OUT0_REG, 1);
}

uint8_t	compose_segments(uint8_t digit, uint8_t with_comma) {
	uint8_t segments = 0x00;
	switch (digit) {
		case 0:
		case '0':
			segments = (uint8_t)~(_BV(disp_g) | _BV(disp_dot));
			break;
		case 1:
		case '1':
			segments = _BV(disp_b) | _BV(disp_c);
			break;
		case 2:
		case '2':
			segments = (uint8_t)~(_BV(disp_c) | _BV(disp_f) | _BV(disp_dot));
			break;
		case 3:
		case '3':
			segments = (uint8_t)~(_BV(disp_e) | _BV(disp_f) | _BV(disp_dot));
			break;
		case 4:
		case '4':
			segments = _BV(disp_b) | _BV(disp_c) | _BV(disp_f) | _BV(disp_g);
			break;
		case 5:
		case '5':
			segments = (uint8_t)~(_BV(disp_b) | _BV(disp_e) | _BV(disp_dot));
			break;
		case 6:
		case '6':
			segments = (uint8_t)~(_BV(disp_b) | _BV(disp_dot));
			break;
		case 7:
		case '7':
			segments = _BV(disp_a) | _BV(disp_b) | _BV(disp_c);
			break;
		case 8:
		case '8':
			segments = (uint8_t)~_BV(disp_dot);
			break;
		case 9:
		case '9':
			segments = (uint8_t)~(_BV(disp_e) | _BV(disp_dot));
			break;
	}
	return (with_comma ? 0x80 : 0x00) | segments;
}

int main(void) {
	uint16_t	count = 0;

	uart_init(115200);
	i2c_init(100000);
	disp_init();
	while (1){
		if (count > 9999)
			count = 0;
		for (uint8_t i = 105; i > 0; --i) {
			disp_set(disp_d1, compose_segments(count % 10000 / 1000, 0));
			disp_set(disp_d2, compose_segments(count % 1000 / 100, 0));
			disp_set(disp_d3, compose_segments(count % 100 / 10, 0));
			disp_set(disp_d4, compose_segments(count % 10, 0));
			}
		++count;
	}
}