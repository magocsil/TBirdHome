#ifndef TBIRD_KEYBOARD
#define TBIRD_KEYBOARD

#ifndef F_CPU
#define F_CPU 8000000
#endif

#include <avr/io.h>
#include <util/delay.h>

void keyboardInit(void)
{
        DDRC &= 0x80;
        DDRC |= 0x78;
}

unsigned short int keyboardRead(void)
{
        unsigned short int result = 0;

        for(int i = 0; i < 4; i++)
        {
                PORTC &= 0x87; 
                PORTC |= (0x08 << i); 
                _delay_ms(3);
                result |= (((~PINC) & 0x07) << (i * 3)); 
                _delay_ms(3);
        }

        return result;
}

#endif
