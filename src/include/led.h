#ifndef TBIRD_LED
#define TBIRD_LED

#include <avr/io.h>

#define EXT_PB6 (1 << 2)
#define EXT_PB7 (1 << 3)
#define EXT_PD4 (1 << 4)

void ledInit(unsigned char toOmit)
{
	unsigned char mask = ~toOmit;
	DDRB |= (mask << 4); //lower four
	DDRD |= (mask & 0xF0); //upper four
}

void ledPutc(unsigned char c, unsigned char toOmit)
{
	PORTB &= (toOmit << 4);
	PORTD &= (toOmit & 0xF0);
	unsigned char masked = c & ~toOmit;
	PORTB |= masked << 4;
	PORTD |= masked & 0xF0;
}

#endif
