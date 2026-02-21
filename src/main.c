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

char timeInput(unsigned short int keyboard, unsigned short int keyboardPrev, unsigned char* digit, char* timeRegister)
{
	unsigned char keyPressesCounter = *digit;
	unsigned char calcNumber;
	for(calcNumber = 0; keyboard >> calcNumber; calcNumber++);
	if((calcNumber == 11) || (calcNumber == 10))
	{
		calcNumber = 0;
	}
	else if(calcNumber == 12)
	{
		lcdCmd(0x0C);
		rtcInit(timeRegister[0] * 16 + timeRegister[1], timeRegister[2] * 16 + timeRegister[3], timeRegister[4] * 16 + timeRegister[5]);
		COUNTDOWN_4_MILLISECONDS = 750;
		*digit = 0;
		return 1;
	}
	
	if(((keyPressesCounter == 1) && (calcNumber > 3) && (timeRegister[0] == 2)) || (!keyPressesCounter && (calcNumber > 2)) || (!(keyPressesCounter & 1) && (calcNumber > 5)))
	{
		*digit = keyPressesCounter;
		return -1;
	}
	timeRegister[keyPressesCounter++] = calcNumber;
	lcdPutc(calcNumber + '0');
	if((keyPressesCounter == 2) || (keyPressesCounter == 4))
	{
		lcdPutc(':');
	}
	else if(keyPressesCounter == 6)
	{
		keyPressesCounter = 0;
		lcdPos(2, 8);
	}
	
	*digit = keyPressesCounter;
	return -1;
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
	lcdPuts("06:30:00");

	unsigned char buttonsPrev = 0;
	unsigned short int keyboardPrev = 0;
	char requestedTime[6] = {0, 6, 3, 0, 0, 0};
	char requestedAlarm[6] = {0, 6, 3, 0, 0, 0};
	char isAlarmEnabled = 0;
	unsigned char keyPressesCounter = 0;

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
		unsigned short int keyboard = 0;

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
			lcdPos(2, 8);
			lcdCmd(0x0D);
			keyPressesCounter = 0;
			mode = -1;
		}
		else if((buttons == 0x04) && !buttonsPrev && (mode == -1))
		{
			lcdCmd(0x0C);
			keyPressesCounter = 0;
			mode = 0;
		}
		else if((mode == -1) && keyboardRead() && !keyboardPrev)
		{
			keyboard = keyboardRead();
			mode = timeInput(keyboard, keyboardPrev, &keyPressesCounter, requestedTime);
			if(mode == 1)
			{
				lcdPos(2, 7);
				char alarmMarker = isAlarmEnabled ? 0b11110100 : ' ';
				lcdPutc(alarmMarker);
				for(int i = 0; (i < 6) && (mode == 1); i++)
				{
					lcdPutc(requestedAlarm[i] + '0');
					if((i == 1) || (i == 3))
					{
						lcdPutc(':');
					}
				}
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
		else if((buttons == 0x01) && !buttonsPrev && (mode == 0))
		{
			lcdPos(2, 8);
			lcdCmd(0x0D);
			keyPressesCounter = 0;
			mode = -2;
		}
		else if((buttons == 0x01) && !buttonsPrev && (mode == -2))
		{
			lcdCmd(0x0C);
			keyPressesCounter = 0;
			mode = 0;
			isAlarmEnabled = 0;
			lcdPos(2, 7);
			lcdPutc(' ');
		}
		else if((mode == -2) && keyboardRead() && !keyboardPrev)
		{
			keyboard = keyboardRead();
			mode = timeInput(keyboard, keyboardPrev, &keyPressesCounter, requestedAlarm);
			if(mode == 1)
			{
				mode = 0;
				lcdPos(2, 7);
				lcdPutc(0b11110100);
				for(int i = 0; (i < 6) && (mode == 1); i++)
				{
					lcdPutc(requestedAlarm[i] + '0');
					if((i == 1) || (i == 3))
					{
						lcdPutc(':');
					}
				}
			}
			else if(mode == -1)
			{
				mode = -2;
			}
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
