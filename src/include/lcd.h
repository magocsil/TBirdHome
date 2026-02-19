#ifndef TBIRD_LCD
#define TBIRD_LCD

#ifndef F_CPU
#define F_CPU 8000000
#endif

#include <avr/io.h>
#include <util/delay.h>

unsigned char lcdPulse(void)
{
        PORTF |= 0x08;
        unsigned char toReturn = 0;
        PORTF &= 0xF7;
        return toReturn;
}

void lcdCmd(unsigned char commandToSend)
{
        PORTF &= 0xFD;

        unsigned char xH = commandToSend & 0xF0;
        unsigned char xL = commandToSend << 4;

        PORTE &= 0x0F;
        PORTE |= xH;
        lcdPulse();

        PORTE &= 0x0F;
        PORTE |= xL;
        lcdPulse();
        _delay_us(40);

        if(commandToSend < 4)
        {
                _delay_ms(5);
        }
}

void lcdInit(void)
{
        DDRF |= 0x0E;
        DDRE |= 0xF0;

        PORTF &= 0xF9;
        _delay_ms(50);

        PORTE &= 0x0F;
        PORTE |= 0x30;
        lcdPulse();
        _delay_ms(5);
        lcdPulse();
        _delay_us(150);
        lcdPulse();
        _delay_us(40);

        PORTE &= 0x0F;
        PORTE |= 0x20;
        lcdPulse();
        _delay_us(40);

        lcdCmd(0x28);
        lcdCmd(0x08);
        lcdCmd(0x01);
        lcdCmd(0x06);
        lcdCmd(0x0C);
}

void lcdAddr(unsigned char pos)
{
        lcdCmd(0x80 + pos); //line 0: 0x80...0x8F; line 2: 0x90...0x9F; line 1:>
}

void lcdPos(unsigned char row, unsigned char col)
{
        switch(row)
        {
                case 0:
                        lcdCmd(0x80 + col);
                        break;
                case 1:
                        lcdCmd(0xC0 + col);
                        break;
                case 2:
                        lcdCmd(0x90 + col);
                        break;
                case 3:
                        lcdCmd(0xD0 + col);
                        break;
        }
}

void lcdPutc(char c)
{
        PORTF |= 0x02;

        char xH = c & 0xF0;
        char xL = c << 4;

        PORTE &= 0x0F;
        PORTE |= xH;
        lcdPulse();

        PORTE &= 0x0F;
        PORTE |= xL;
        lcdPulse();
        _delay_us(40);
}

void lcdPuts(char* s)
{
        for(char i = 0; *(s + i); i++)
        {
                if(i == 8)
                {
                        lcdAddr(0x40);
                }

                lcdPutc(*(s + i));
        }
}

#endif
