#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

typedef uint8_t bool;
#define true 1;
#define false 0;

void	uart_init(uint32_t baudrate);
void	uart_tx(uint8_t character);
void	uart_putnbr(uint32_t nb);
void	uart_puthex(uint8_t c);
void	uart_putstr(volatile uint8_t* string);

bool	safe_eeprom_read(void* buffer, size_t offset, size_t length) {										// Returns true if we actually managed to read something
	if (((uint16_t)eeprom_read_byte((uint8_t*)offset) << 8 | (uint16_t)eeprom_read_byte((uint8_t*)offset + 1)) == 0xBEEF) {		// If we can find our magic number at offset, clear out the space we will be using.
		for (size_t i = 0; i < length; i++) {
			((uint8_t*)buffer)[i] = eeprom_read_byte((uint8_t*)offset + 2 + i);
		}
		return true;
	}
	return false;
}

bool	safe_eeprom_write(void* buffer, size_t offset, size_t length) {										// Returns true if we actually had to write something
	bool res = false;
	if (((uint16_t)eeprom_read_byte((uint8_t*)offset) << 8 | (uint16_t)eeprom_read_byte((uint8_t*)offset + 1)) != 0xBEEF) {
		res = true;
		eeprom_write_byte((uint8_t*)offset, 0xBE);
		eeprom_write_byte((uint8_t*)offset + 1, 0xEF);
	}
	for (size_t i = 0; i < length; i++) {
		if (eeprom_read_byte((uint8_t*)offset + 2 + i) != ((uint8_t*)buffer)[i]) {
			res = true;
			eeprom_write_byte((uint8_t*)offset + 2 + i, ((uint8_t*)buffer)[i]);
		}
	}
	return res;
}

int main(void) {
	uart_init(115200);

	uint8_t	buf[255] = {'b', 'o', 'j', 'o', 'u', 'r', 'e', ' ', 0};

	uart_puthex(safe_eeprom_read(buf, 0x0F, 15));
	uart_puthex(safe_eeprom_write(buf, 0x0F, 9));
	uart_puthex(safe_eeprom_read(buf, 0x0F, 15));
	uart_putstr(buf);
	uart_puthex(safe_eeprom_write(buf, 0x0F, 9));
	while (1);
}


	