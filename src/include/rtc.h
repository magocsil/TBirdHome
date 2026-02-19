#ifndef TBIRD_RTC
#define TBIRD_RTC

void rtcInit(void)
{
	i2cStart((0x51 << 1), TW_WRITE);
        i2cWrite(0x2);
        i2cWrite(0x20);
        i2cWrite(0x24);
        i2cWrite(0x02);
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
