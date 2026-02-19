#ifndef TBIRD_DHT11
#define TBIRD_DHT11

#ifndef F_CPU
#define F_CPU 8000000
#endif

#include <avr/io.h>
#include <util/delay.h>

unsigned short int COUNTDOWN_4_MILLISECONDS_2 = 0;

void dht11Init(void)
{
	DDRB |= 0x40;
	PORTB |= 0x40;
}

unsigned long int dht11Read(void)
{
	unsigned long int data = 0;
	unsigned char checksum = 0;
	unsigned char sumToCheck = 0;
	PORTB &= 0xBF;
	_delay_ms(20);
	PORTB |= 0x40;
	DDRD &= 0xBF;
	while(!(PINB & 0x40));
	_delay_us(100);
	if(!(PINB & 0x40)) return 0;
	while(PINB & 0x40);
	
	for(int i = 40; i > 0; i--)
	{
		while(!(PINB & 0x40));
		COUNTDOWN_4_MILLISECONDS_2 = 13;
		while((PINB & 0x40) && (i > 8))
		{
			data |= (1 << i);
			unsigned char currentBitIndex = i % 8;
			if(currentBitIndex - 1 <= 1)
			{
				sumToCheck |= (1 << (i / 4 - 3 + currentBitIndex));
			}
		}
		while((PINB & 0x40) && (i <= 8)) checksum |= (1 << i);
	}
	while(!(PINB & 0x40));
	dht11Init();
	
	if(sumToCheck == checksum) return data;
	return checksum | (sumToCheck << 8);
}

#endif
