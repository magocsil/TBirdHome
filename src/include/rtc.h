#ifndef TBIRD_RTC
#define TBIRD_RTC

void rtcInit(unsigned char hours, unsigned char minutes, unsigned char seconds)
{
	i2cStart((0x51 << 1), TW_WRITE);
        i2cWrite(0x2);
        i2cWrite(seconds);
        i2cWrite(minutes);
        i2cWrite(hours);
        i2cStop();
}

unsigned long int rtcRead(void)
{
	
        i2cStart((0x51 << 1), TW_WRITE);
        i2cWrite(0x2);
        i2cStop();
        i2cStart((0x51 << 1), TW_READ);
	
	unsigned long int rtcTime = 0;
        rtcTime |= i2cRead(1);
        rtcTime |= (((unsigned long int)i2cRead(1)) << 8);
        rtcTime |= (((unsigned long int)i2cRead(0)) << 16);
        i2cStop();
	
	return rtcTime;
}

#endif
