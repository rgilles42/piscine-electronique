NAME=main

CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-Os -DF_CPU=16000000UL -mmcu=atmega328p

PORT= /dev/ttyUSB0

SOURCES = main.c
OBJS = $(subst .c,.o,$(SOURCES))

all: hex flash connect

hex: ${OBJS}
	${CC} ${CFLAGS} -o ${NAME}.bin ${OBJS}
	${OBJCOPY} -O ihex ${NAME}.bin ${NAME}.hex

flash: 
	avrdude -c arduino -p ATMEGA328P -P ${PORT} -b 115200 -U flash:w:${NAME}.hex

connect:
	screen /dev/ttyUSB0 115200,cs8

clean:
	rm -rf *.o *.bin

fclean: clean
	rm -rf *.hex

re: fclean all

.PHONY: hex flash connect all clean fclean re