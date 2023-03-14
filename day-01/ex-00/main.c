#include <avr/io.h>


int main(void) {
    DDRB = 1 << PB1;                                    // Set OC1A/PB1 as output.

    TCCR1A = 0;                                         // Clear TCCR1A
    TCCR1B = 0;                                         // Clear TCCR1B
    TCCR1C = 0;                                         // Clear TCCR1C
    TCNT1 = 0;                                          // Initialise TC1 value to 0.

    TCCR1B |= (1 << WGM12);                             // Set TC1 to CTC mode by flipping WGM12 bit.
    TCCR1A |= (1 << COM1A0);                            // Toggle OC1A/PB1 when compare is true.
    OCR1A = (31249);                                    // With a pre-scaler at 256, it takes 31250 ticks to reach 1sec.
    TCCR1B = (1 << CS12);                               // Set TC1 pre-scaler up to 256, start the timer

    while (1) {
        ;
    }
}
