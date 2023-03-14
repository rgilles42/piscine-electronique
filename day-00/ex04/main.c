#include <avr/io.h>
#include <util/delay.h>

void wait_millisec(void) {
    for (unsigned long i = 0; i < 2286; i++) // This loop is taking 7 MCU cycles -> 0.001/(7/16000000) = 2.287,7
        __asm__ __volatile__("nop");
}

int main(void){
    DDRB = 0xFF;            // Configure PORTB as output
    DDRD = 0x00;            // Configure PORTD as input
    PORTB = 0x00;           // Initialize PORTB pins to OFF

    while (1){
        if ((PIND & (1 << 2)) ^ 1 << 2) {       // If PD2 is low = pressed
            PORTB ^= 1;                         // Flip PB0
            wait_millisec();                    // Wait to ignore bounce effect
            while ((PIND & (1 << 2)) ^ 1 << 2);  // Wait for button release
        }
        wait_millisec();                        // Wait to ignore bounce effect
    }
    return 0;
}