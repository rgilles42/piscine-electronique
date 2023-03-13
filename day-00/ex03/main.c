#include <avr/io.h>
#include <util/delay.h>

int main(void){
    DDRB = 0xFF;            // Configure PORTB as output
    DDRD = 0x00;            // Configure PORTD as input

    while (1)
        PORTB = ((PIND & (1 << 2)) ^ 1 << 2) >> 2; // AND the PIND value with 0b0000100 to isolate value of PD2; flip it, then shift right it by 2 to place it at 0b000000X
    return 0;
}