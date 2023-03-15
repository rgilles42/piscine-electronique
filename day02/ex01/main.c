#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

ISR(TIMER0_OVF_vect) {                                          // We know that our overflow interrupt will be triggered each millisecond
    static uint8_t is_rising = 1;

    if (is_rising)
        OCR1A += 1;                                             // We want the PWM duty cycle to grow from 0 to 16000 in 500 ms => 16000/500 = 32
    else
        OCR1A -= 1;                                             // idem but in the other direction
    if (OCR1A == 100 || OCR1A == 0)                             // If we are at 0% or 100% duty cycle, change direction. 
        is_rising ^=1;
}

int main(void) {
    DDRB = _BV(PB1);                                            // Set OC1A/PB1 as output.

    TCCR0A = 0;                                                 // Clear TCCR0A
    TCCR0B = 0;                                                 // Clear TCCR0B
    TIMSK0 = 0;                                                 // Clear TIMSK0
    TCNT0 = 0;                                                  // Initialise TC0 value to 0.
    TCCR1A = 0;                                                 // Clear TCCR1A
    TCCR1B = 0;                                                 // Clear TCCR1B
    TIMSK1 = 0;                                                 // Clear TIMSK1
    TCNT1 = 0;                                                  // Initialise TC1 value to 0.

    ICR1 = 100;                                                 // Set ICR1/TOP at 100 ticks (with a pre-scaler at 1, it represents a heckin short period). Our PWM signal period will be this heckin short period.
    OCR1A = 0;                                                  // Set the Compare A value at 0% of TOP value
    TCCR1A |= _BV(COM1A1) | _BV(WGM11);                         // Set Compare Output A Mode to COM1A1:0 = 10 (Set OCA1 at BOTTOM, clear it at Compare); Set TC1 mode to Fast PWM ICR1 (WGM13:0 = 14)
    TCCR1B |= _BV(WGM13) | _BV(WGM12) | _BV(CS10);              // Set TC1 mode to Fast PWM ICR1 (WGM13:0 = 14); Set up TC1 pre-scaler to 1 (this starts the timer)

    OCRA = 250;                                                 // Set OCRA/TOP at 250 ticks (with prescaler at 64, it represents 1ms).
    TIMSK0 |= _BV(TOIE0);                                       // Trigger interrupt at each overflow of TC0 (each millisecond)
    TCCR0A |= _BV(WGM01) | _BV(WGM00);                          // Set TC0 mode to Fast PWM OCRA (WGM02:0 = 7)
    TCCR0B |= _BV(WGM02) | _BV(CS01) | _BV(CS00);               // Set TC0 mode to Fast PWM OCRA (WGM02:0 = 7); Set up TC1 pre-scaler to 64 (this starts the timer)

    sei();
    while (1);
}


    