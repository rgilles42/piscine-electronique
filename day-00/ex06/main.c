#include <avr/io.h>
#include <util/delay.h>

void wait_millisec(void) {
    for (unsigned long i = 0; i < 246154; i++) // This loop is taking 6.5 MCU cycles and we need to wait 0.1s -> 0.1/(6.5/16000000) = 2,46153.8
        __asm__ __volatile__("nop");
}

int main(void){
    DDRB = 0xFF;     // Configure PORTB as output
    PORTB = 1;       // Initialize PORTB pins to OFF

    while (1){
        wait_millisec();
        PORTB = PORTB << 1;
        wait_millisec();
        PORTB = PORTB << 1;
        wait_millisec(); 
        PORTB = PORTB << 2;
        wait_millisec();
        PORTB = PORTB >> 2;
        wait_millisec();
        PORTB = PORTB >> 1;
        wait_millisec();
        PORTB = PORTB >> 1;
    }
    return 0;
}