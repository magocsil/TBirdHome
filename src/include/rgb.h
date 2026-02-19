#ifndef TBIRD_RGB
#define TBIRD_RGB

#include <avr/io.h>

#define RGB_R (1 << 7)
#define RGB_G (1 << 3)
#define RGB_B (1 << 2)
#define RGB_ALL (0x8C)

void rgbInit(void)
{
	DDRC |= 0x80;
	DDRE |= 0x0C;
}

void rgbSet(unsigned char colors)
{
	PORTC |= (colors & 0x80);
	PORTE |= (colors & 0x0C);
}

void rgbClear(unsigned char colors)
{
	PORTC &= (~colors & 0x80);
	PORTE &= (~colors & 0x0C);
}

void rgbToggle(unsigned char colors)
{
	unsigned char prev = PORTC;
	PORTC &= ~0x80;
	PORTC |= (~prev & colors & 0x80);
	prev = PORTE;
	PORTE &= ~0x0C;
	PORTE |= (~prev & colors & 0x0C);
}

#endif
