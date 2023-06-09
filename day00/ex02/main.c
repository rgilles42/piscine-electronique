#include <avr/io.h>

void wait_half_sec(void) {
    for (unsigned long i = 0; i < 1142857; i++) // This loop is taking 7 MCU cycles -> 0.5/(7/16000000) = 1.142.857,143
        __asm__ __volatile__("nop");
}

int main(void){
    DDRB = 0xFF;            // Configure PORTB as output
    PORTB = 0b00000001;     // Turn ON the LED connected to pin 0 of PORTB
    while (1) {
        wait_half_sec();
        PORTB ^= 0b00000001; // Flip the LED connected to pin 0 of PORTB
    
    }
    return 0;
}