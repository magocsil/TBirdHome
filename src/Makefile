all: build

build: main.elf

main.elf: main.c
	avr-gcc -Wall -g -Os -mmcu=atmega128 -o main.elf main.c

flash: main.hex
	avrdude -p atmega128 -c jtag1 -U flash:w:main.hex -P /dev/ttyUSB0

main.hex: main.elf
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex

clean:
	rm *.hex *.elf
