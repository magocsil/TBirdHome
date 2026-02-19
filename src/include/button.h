#ifndef TBIRD_BUTTON
#define TBIRD_BUTTON

#include <avr/io.h>

void buttonInit(void)
{
        DDRG &= 0xE0;
}

unsigned char buttonRead(void)
{
        return (PING & 0x1F);
}

#endif
