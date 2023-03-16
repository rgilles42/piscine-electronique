#include <avr/io.h>
void	USART_RX_vect(void) __attribute__ ((signal, used, externally_visible));

volatile uint8_t*	username = "rgilles";
volatile uint8_t*	password = "password";

volatile uint8_t	username_len = 0;
volatile uint8_t	username_prompt[255];

volatile uint8_t	password_len = 0;
volatile uint8_t	password_prompt[255];

volatile uint8_t*	current_len;
volatile uint8_t*	current_prompt;

volatile uint8_t	username_entered = 0;
volatile uint8_t	password_entered = 0;

uint8_t	ft_strcmp(volatile uint8_t* str1, volatile uint8_t* str2) {
	while(*str1 && *str2 && *str1 == *str2 ) {
		++str1;
		++str2;
	}
	return (*str2 - *str1);
}

void	uart_tx(uint8_t character) {
	while (!(UCSR0A & _BV(UDRE0)));					// Wait for the Data Register Empty event.
	UDR0 = character;								// Place character into transmission buffer.
}

void	uart_printstr(volatile uint8_t* string) {
	while (*string) {
		uart_tx(*string);
		++string;
	}
	uart_tx(*string);
}

void 	USART_RX_vect(void) {						
	uint8_t my_byte = UDR0;							// The read byte is available in UDR0.
	if (my_byte == 13){								// If Enter is received.
		if (username_entered)
			++password_entered;
		else
			++username_entered;
		current_prompt[*current_len] = 0;
	}
	else if (my_byte == 127) {						// If DEL is received.
		if (*current_len) {
			uart_tx('\b');							// Print non-destructive backspace
			uart_tx(' ');							// Print space over char to erase
			uart_tx('\b');							// Print non-destructive backspace
			--(*current_len);
		}
	}
	else {
		uart_tx(username_entered ? '*' : my_byte);
		current_prompt[*current_len] = my_byte;
		++(*current_len);
	}
}

void	uart_init(uint32_t baudrate) {
	uint32_t ubbr_value = F_CPU/(16*baudrate);		// witout the -1 because the integer logic truncates instead of rounding
	UBRR0H = (uint8_t) (ubbr_value >> 8);			// Put the upper 8 bit value of UBBR. 
	UBRR0L = (uint8_t) ubbr_value;					// Put the lower 8 bit value of UBBR. 
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);				// Set payload length to 8 bits.
	UCSR0B = _BV(TXEN0) | _BV(RXEN0) | _BV(RXCIE0);	// Enable UART transmission and reception; Enable RX Complete Interrupt.
}

int		main(void) {
	DDRB = _BV(PB4) | 7;							// Set LEDS as output on Port B.
	uart_init(115200);
	__asm__ __volatile__ ("sei" ::: "memory");		// Enable interrupts.
	while(1) {
		uart_printstr("Enter your login:\r\n\tusername: ");
		current_len = &username_len;
		current_prompt = (char*)username_prompt;
		while(!username_entered);
		if (username_len){
			uart_printstr("\r\n\tpassword: ");
			current_len = &password_len;
			current_prompt = (char*)password_prompt;
			while(!password_entered);
			if ((ft_strcmp(username, username_prompt) == 0)
			&& (ft_strcmp(password, password_prompt) == 0)) {
				__asm__ __volatile__ ("cli" ::: "memory");		// Disable interrupts.
				PORTB = _BV(PB4) | 7;
				uart_printstr("\r\nHello ");
				uart_printstr(username_prompt);
				uart_printstr("!\r\nShall we play a game?\r\n");
				while(1);
			}
			password_entered = 0;
			password_len = 0;
			uart_printstr("\r\nWrong username/password");
		}
		username_entered = 0;
		username_len = 0;
		uart_printstr("\r\n\r\n");
	
	}
}