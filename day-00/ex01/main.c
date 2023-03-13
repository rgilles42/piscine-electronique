#include <avr/io.h>

int main(void){
    DDRB = 0xFF;        // Configure PORTB as output
    PORTB = 0b00000001; // Turn ON the LED connected to pin 0 of PORTB
    return 0;
}