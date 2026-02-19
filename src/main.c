#include "include/led.h"
#include "include/rgb.h"
#include "include/sevSeg.h"
#include "include/keyboard.h"
#include "include/button.h"
#include "include/lcd.h"
#include "include/temperatureLM35.h"
#include "include/i2c.h"
#include "include/rtc.h"


#ifndef F_CPU
#define F_CPU 8000000
#endif
#ifndef BAUD
#define BAUD 115200
#endif

void uartInit(void)
{
	UBRR0H = 0;
	UBRR0L = F_CPU / 16 / BAUD - 1;
	UCSR0B  = (1<<RXEN)|(1<<TXEN);
}
void uartPutc(unsigned char c)
{
	while(!(UCSR0A & (1 << UDRE0)));
	
	UDR0 = c;
}

int main(void)
{
	ledInit(0);
	sevSegInit();
	timer0Init();
	buttonInit();
	lcdInit();
	lcdAddr(0x80);
	lm35Init();
	adcInit();
	rgbInit();
	keyboardInit();
	i2cInit();
	rtcInit();

	unsigned char sensor = 2;
	unsigned char mode = 0;

	while(1)
	{
//		rgbSet(RGB_G);
//		ledPutc(keyboardRead() >> 4, 0);
		
		if(buttonRead() >= 0x10)
		{
			mode = 1;
			COUNTDOWN_4_MILLISECONDS = 750;
		}
		else if(buttonRead() >= 0x08)
		{
			mode = sensor;
			COUNTDOWN_4_MILLISECONDS = 750;
		}
		else if(!buttonRead() && !COUNTDOWN_4_MILLISECONDS)
		{
			mode = 0;
			PORTA = 0;
		}

		unsigned long int rtcTime = rtcRead();
		unsigned char seconds = rtcTime;
		unsigned char minutes = (rtcTime >> 8);
		unsigned char hours = (rtcTime >> 16);
		
		if(mode == 1)
		{
			sevSegPutt(hours, minutes, seconds);
		}
		else if(mode > 0)
		{
			sevSegPut2n(TEMPERATURE_LM35, 0);
		}
	}
	
	return 0;
	}
