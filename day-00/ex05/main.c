#include <avr/io.h>
#include <util/delay.h>

void wait_millisec(void) {
    for (unsigned long i = 0; i < 2462; i++) // This loop is apparently taking 6.5 MCU cycles and we need to wait 0.001s -> 0.001/(6.5/16000000) = 2,461.538
        __asm__ __volatile__("nop");
}

int main(void){
    DDRB = 0xFF;            // Configure PORTB as output
    DDRD = 0x00;            // Configure PORTD as input
    PORTB = 0x00;           // Initialize PORTB pins to OFF
    uint8_t counter = 0x00;

    while (1){
        if ((PIND & (1 << 2)) ^ 1 << 2) {       // If PD2 is low = pressed
            counter += 1 ;
            wait_millisec();                    // Wait to ignore bounce effect
            while ((PIND & (1 << 2)) ^ 1 << 2); // Wait for button release
            wait_millisec();                    // Wait to ignore bounce effect
        }
        else if ((PIND & (1 << 4)) ^ 1 << 4) {       // If PD4 is low = pressed
            counter -= 1;
            wait_millisec();                    // Wait to ignore bounce effect
            while ((PIND & (1 << 4)) ^ 1 << 4); // Wait for button release
            wait_millisec();                    // Wait to ignore bounce effect
        }
        PORTB = counter & 0b00000111;
        PORTB |= counter >> 3 << 4;
    }
    return 0;
}