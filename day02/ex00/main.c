#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

ISR(INT0_vect) {
    _delay_ms(1);
    PORTB ^= _BV(PB0);                      // Flip PB0.
}

int main(void) {
    cli();
    DDRB = _BV(PB0);                        // Set PB0 as output.
    PORTB = 0x00;                           // Clear PORTB.

    EICRA = _BV(ISC01);                     // Set External Interrupt Control to Falling Edge mode for INT0.
    EIMSK = _BV(INT0);                      // Declare INT0 in the External Interrupt Mask Register
    sei();
    while(1);
}