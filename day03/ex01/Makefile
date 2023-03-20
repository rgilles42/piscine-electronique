NAME=main

CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS= -DF_CPU=16000000UL -mmcu=atmega328p

PORT= /dev/ttyUSB0


all: hex flash

hex: ${NAME}.o
	${CC} ${CFLAGS} -o ${NAME}.bin ${NAME}.o
	${OBJCOPY} -O ihex ${NAME}.bin ${NAME}.hex

flash: 
	avrdude -c arduino -p ATMEGA328P -P ${PORT} -b 115200 -U flash:w:${NAME}.hex

clean:
	rm -rf *.o *.bin

fclean: clean
	rm -rf *.hex

re: fclean all

.PHONY: hex flash all clean fclean re