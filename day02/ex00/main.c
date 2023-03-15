#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

ISR(INT0_vect) {
    static uint8_t falling_edge = 1;
    if (falling_edge)
        PORTB ^= _BV(PB0);                  // Flip PB0.
    _delay_ms(1);                           // Ignore bounce-back.
    falling_edge ^= 1;                      // Next interrupt will be for rising edge.
    EIFR |= _BV(INT0);                      // Clear the interrupt flag for any bouncing that happened during the pause.
}

int main(void) {
    cli();
    DDRB = _BV(PB0);                        // Set PB0 as output.
    PORTB = 0x00;                           // Clear PORTB.

    EICRA = _BV(ISC00);                     // Set External Interrupt Control to React To Any Logical Change mode for INT0.
    EIMSK = _BV(INT0);                      // Declare INT0 in the External Interrupt Mask Register
    sei();
    while(1);
}