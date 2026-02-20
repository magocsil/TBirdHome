//#define F_CPU 8000000

#include "include/led.h"
#include "include/rgb.h"
#include "include/sevSeg.h"
#include "include/keyboard.h"
#include "include/button.h"
#include "include/dht11.h"
#include "include/lcd.h"
#include "include/lm35.h"
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
	ledInit(EXT_PB6);
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
	rtcInit(6, 0, 0);
	dht11Init();

	char mode = 0;
	char sensor = 2;
	lcdPos(3, 7);
	lcdPutc(0b11011111);
	lcdPuts("C:  LM35");

	unsigned short int outputDHT11 = 0;
	unsigned char humidityDHT11 = 0;
	unsigned char temperatureDHT11 = 0;
	
	lcdPos(2, 8);
//	lcdPutc(0b11110100);
	lcdPuts("06:30:00");
	unsigned char keyPressesCounter = 0;

	unsigned char buttonsPrev = 0;
	unsigned char keyboardPrev = 0;
	char requestedNumbers[6];

	while(1)
	{
		if(DHT11_START)
		{
			outputDHT11 = dht11Read();
			humidityDHT11 = ((unsigned short int)outputDHT11 >> 8);
			temperatureDHT11 = outputDHT11;
			DHT11_START = 0;
		}
		unsigned char buttons = buttonRead();
		unsigned char keyboard = 0;

		if((buttons == 0x10) && (mode >= 0))
		{
			mode = 1;
			COUNTDOWN_4_MILLISECONDS = 750;
		}
		else if((buttons == 0x08) && (mode >= 0))
		{
			mode = sensor;
			COUNTDOWN_4_MILLISECONDS = 750;
		}
		else if((buttons == 0x04) && !buttonsPrev && (mode == 0))
		{
			lcdPos(2, 6);
			lcdPutc(0b10100100);
			lcdPutc(' ');
			mode = -1;
			
			if(keyboardRead() && !keyboardPrev)
			{
				keyboard = keyboardRead();
				unsigned char calcLetter;
				for(calcLetter = 0; keyboard >> calcLetter; calcLetter++);
				if((calcLetter == 11) || (calcLetter == 10))
				{
					calcLetter = 0;
				}
				else if(calcLetter == 12)
				{
					i2cStop();
					rtcInit(requestedNumbers[0] * 16 + requestedNumbers[1], requestedNumbers[2] * 16 + requestedNumbers[3], requestedNumbers[4] * 16 + requestedNumbers[5]);
					mode = 1;
					COUNTDOWN_4_MILLISECONDS = 750;
				}

				if(((keyPressesCounter == 1) && (calcLetter > 3) && (requestedNumbers[0] < 2)) || (!keyPressesCounter && calcLetter > 2) || (!(keyPressesCounter & 1) && (calcLetter > 5)))
				{
					continue;
				}
				lcdPos(2, 8 + keyPressesCounter);
				if((keyPressesCounter == 2) || (keyPressesCounter == 5)) lcdPutc(':');
				requestedNumbers[keyPressesCounter++] = calcLetter;
				lcdPutc(calcLetter);
				if(keyPressesCounter == 6) keyPressesCounter = 0;
			}
		}
		else if((buttons == 0x02) && !buttonsPrev)
		{
			sensor ^= 1;
			lcdPos(3, 11);
			if(sensor & 1)
			{
				lcdPuts("DHT11");
			}
			else
			{
				lcdPuts(" LM35");
			}
		}
		else if(!buttons && mode == -1)
		{
		
		}
		else if(!buttons && !COUNTDOWN_4_MILLISECONDS)
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
			switch(sensor)
			{
				case 2:
					sevSegPut2n(TEMPERATURE_LM35, humidityDHT11);
					break;
				case 3:
					sevSegPut2n(temperatureDHT11, humidityDHT11);
					break;
			}
		}
		
		buttonsPrev = buttons;
		keyboardPrev = keyboard;
	}
	return 0;
}
