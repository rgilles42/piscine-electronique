#include <avr/io.h>
#include <util/delay.h>

void wait_millisec(void) {
    for (unsigned long i = 0; i < 228571; i++) // This loop is taking 7 MCU cycles -> 0.1/(7/16000000) = 228571.4
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