#ifndef TBIRD_DHT11
#define TBIRD_DHT11

#ifndef F_CPU
#define F_CPU 8000000
#endif

#include <avr/io.h>
#include <util/delay.h>

void dht11Init(void)
{
	DDRB |= 0x40;
	PORTB |= 0x60;
}

unsigned short int dht11Read(void)
{
	unsigned short int data = 0;
	PORTB &= 0xBF;
	_delay_ms(20);
	PORTB |= 0x40;
	DDRB &= 0xBF;
	while(!(PINB & 0x40));
	_delay_us(100);
	while(PINB & 0x40);
	
	for(int i = 23; i >= 0; i--)
	{
		while(!(PINB & 0x40));
		_delay_us(35);
		while(PINB & 0x40)
		{
			if(i >= 16) data |= (1 << (i - 8));
			else if(i < 7) data |= (1 << i);
		}
	}
	_delay_ms(2);
	while(!(PINB & 0x40));
	dht11Init();
	
	return data;
}

#endif
