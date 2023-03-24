#include <avr/io.h>


int main(void) {
    DDRB = 1 << PB1;                                    // Set OC1A/PB1 as output.

    TCCR1A = 0;                                         // Clear TCCR1A
    TCCR1B = 0;                                         // Clear TCCR1B
    TCNT1 = 0;                                          // Initialise TC1 value to 0.

    OCR1A = (31249);                                    // Set the Compare A value at 31250 ticks (with a pre-scaler at 256, it takes 62500 ticks to reach 1sec)
    TCCR1A |= (1 << COM1A0);                            // Set Compare Output A Mode to 01 (Toggle OC1A/PB1 at Compare).
    TCCR1B |= (1 << WGM12) | (1 << CS12);               // Set TC1 to CTC OCR1A mode (mode 9); Set TC1 pre-scaler up to 256 (this starts the timer)

    while (1) {
        ;
    }
}
