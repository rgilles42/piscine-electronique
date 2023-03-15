#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint8_t flag = 0;

ISR(TIMER1_OVF_vect) {                                          // We know that our overflow interrupt will be triggered each millisecond
    static uint8_t is_rising = 0;

    if (OCR1A == 16000 || OCR1A == 0)                           // If we are at 0% or 100% duty cycle, go back. 
        is_rising ^=1;
    if (is_rising)
        OCR1A += 32;                                            // We want the PWM duty cycle to grow from 0 to 16000 in 500 ms => 16000/500 = 32
    else
        OCR1A -= 32;                                            // idem but in the other direction
}

int main(void) {
    DDRB = _BV(PB1);                                            // Set OC1A/PB1 as output.

    TCCR1A = 0;                                                 // Clear TCCR1A
    TCCR1B = 0;                                                 // Clear TCCR1B
    TIMSK1 = 0;                                                 // Clear TIMSK1
    TCNT1 = 0;                                                  // Initialise TC1 value to 0.

    ICR1 = 16000;                                               // Set ICR1/TOP at 16000 ticks (with a pre-scaler at 1, it represents 1ms). Our PWM signal period will be of 1ms.
    OCR1A = 0;                                                  // Set the Compare A value at 0% of TOP value
    TIMSK1 |= _BV(TOIE1);                                       // Enable TC1 Overflow interrupts.
    TCCR1A |= _BV(COM1A1) | _BV(WGM11);                         // Set Compare Output A Mode to COM1A1:0 = 10 (Set OCA1 at BOTTOM, clear it at Compare); Set TC1 mode to Fast PWM ICR1 (WGM13:0 = 14)
    TCCR1B |= _BV(WGM13) | _BV(WGM12) | _BV(CS10);              // Set TC1 mode to Fast PWM ICR1 (WGM13:0 = 14); Set up TC1 pre-scaler to 1 (this starts the timer)
    sei();
    while (1);
}


    