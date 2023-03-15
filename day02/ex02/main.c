#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint8_t counter = 0x00;

ISR(INT0_vect) {
    static uint8_t falling_edge = 1;
    if (falling_edge){
        counter += 1 ;
        PORTB = counter & 0b00000111;
        PORTB |= counter >> 3 << 4;
    }
    _delay_ms(1);                                               // Ignore bounce-back.
    falling_edge ^= 1;                                          // Next interrupt will be for rising edge.
    EIFR |= _BV(INT0);                                          // Clear the interrupt flag for any bouncing that happened during the pause.
}

ISR(PCINT2_vect) {
    static uint8_t falling_edge = 1;
    if (falling_edge){
        counter -= 1 ;
        PORTB = counter & 0b00000111;
        PORTB |= counter >> 3 << 4;
    }
    _delay_ms(1);                                               // Ignore bounce-back.
    falling_edge ^= 1;                                          // Next interrupt will be for rising edge.
    EIFR |= _BV(INT0);                                          // Clear the interrupt flag for any bouncing that happened during the pause.
}


int main(void) {

    DDRB = _BV(PB0) | _BV(PB1) | _BV(PB2) | _BV(PB4);           // Configure PORTB as output
    DDRD = 0x00;                                                // Configure PORTD as input
    PORTB = 0x00;                                               // Initialize PORTB pins to OFF

    EICRA = _BV(ISC00);                                         // Set External Interrupt Control to React To Any Logical Change mode for INT0.
    EIMSK = _BV(INT0);                                          // Declare INT0 in the External Interrupt Mask Register

    PCICR = _BV(PCIE2);                                         // Enable Pin Change Interrupt for PCINT pin group 2.
    PCMSK2 = _BV(PCINT20);                                      // Enable PCINT20 in the Pin Change Mask Register for PCINT pin group 2
    sei();
    while (1);
}


    