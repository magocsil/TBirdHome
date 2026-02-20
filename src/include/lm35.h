#ifndef TBIRD_LM35
#define TBIRD_LM35

#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char TIM0_MATCH_COUNT = 0;
unsigned short int COUNTDOWN_4_MILLISECONDS = 0;
unsigned char TEMPERATURE_LM35;
unsigned char DHT11_START = 0;

void timer0Init(void)
{
        TCCR0 |= (1 << WGM01) | 6; //Clear Timer on Compare match and prescaler>
        OCR0 = 250;
        TIMSK |= (1 << OCIE0);
        sei();
}

void adcInit(void)
{
        ADMUX = 0; //4.096 V AREF (with internal VREF off), right-adjusted, ADC>
        ADCSRA = ~0x20; //ADC enabled, conversion started, free running off, in>
        sei();
}

void adcStartConversion(void)
{
        ADCSRA |= (1 << ADSC);
}

unsigned short int adcRead(void)
{
        unsigned short int result = ADCL;
        result |= ((ADCH & 0x03) << 8);

        return result;
}

void lm35Init(void)
{
        DDRF &= 0xFE;
}

ISR(TIMER0_COMP_vect)
{
        if(COUNTDOWN_4_MILLISECONDS)
        {
                COUNTDOWN_4_MILLISECONDS--;
        }
	
        if(TIM0_MATCH_COUNT++ >= 250)
        {
                adcStartConversion();
		DHT11_START = 1;
                TIM0_MATCH_COUNT = 0;
        }
}

ISR(ADC_vect)
{
        TEMPERATURE_LM35 = adcRead() * 4 / 10;
/*        char temperatureString[6] = "    C";
        temperatureString[0] = TEMPERATURE_LM35 / 10;
        temperatureString[1] = TEMPERATURE_LM35 - temperatureString[0] * 10 + '0';
        temperatureString[0] += '0';
        temperatureString[3] = 0b11011111;
        lcdPos(2, 0);
        lcdPuts(temperatureString);
*/}

#endif
