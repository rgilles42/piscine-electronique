#include <avr/io.h>


int main(void) {
    DDRB = 1 << PB1;                                            // Set OC1A/PB1 as output.

    TCCR1A = 0;                                                 // Clear TCCR1A
    TCCR1B = 0;                                                 // Clear TCCR1B
    TCNT1 = 0;                                                  // Initialise TC1 value to 0.

    ICR1 = 62499;                                              // Set ICR1/TOP at 62500 ticks (with a pre-scaler at 256, it takes 62500 ticks to reach 1sec)
    OCR1A = 6250;                                              // Set the Compare A value at 10% of TOP value
    TCCR1A |= (1 << COM1A1) | (1 << WGM11);                    // Set Compare Output A Mode to 10; Set mode to Fast PWM 14
    TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS12);       // Set mode to Fast PWM 14; Set TC1 pre-scaler up to 256; start the timer

    while (1) {
        ;
    }
}
