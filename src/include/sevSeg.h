#ifndef TBIRD_SEV_SEG
#define TBIRD_SEV_SEG

#ifndef F_CPU
#define F_CPU 8000000
#endif

#include <avr/io.h>
#include <util/delay.h>

void sevSegInit(void)
{
	DDRA = 0xFF;
	PORTA |= 0x80;
}

void sevSegPutd(unsigned char select, unsigned char digit)
{
	if(digit > 9)
	{
		digit = 0;
	}
	PORTA &= 0x80;
	PORTA |= 0x80 | ((select & 0x07) << 4) | (digit & 0x0F);
}

void sevSegPutn(unsigned char select, unsigned char num)
{
	unsigned char selCorrected = select;
	if(select & 1)
	{
		selCorrected--;
	}
	sevSegPutd(selCorrected + 1, num / 10);
	_delay_ms(3);
	sevSegPutd(selCorrected, num % 10);
	_delay_ms(3);
}

void sevSegPut2n(unsigned char num1, unsigned char num2)
{
	sevSegPutn(2, num1);
	sevSegPutn(0, num2);
}

void sevSegPutt(unsigned char hours, unsigned char minutes, unsigned char seconds)
{
        sevSegPutd(3, ((hours >> 4) & 0x03));
        _delay_ms(3);
        sevSegPutd(2, (hours & 0x0F));
        _delay_ms(3);
        sevSegPutd(1, ((minutes >> 4) & 0x07));
        _delay_ms(3);
        sevSegPutd(0, (minutes & 0x0F));
        _delay_ms(3);
        if(seconds & 1)
        {
                PORTA &= 0x80;
                PORTA |= 0x4F;
                _delay_ms(3);
        }
}

#endif
