#ifndef TBIRD_I2C
#define TBIRD_I2C

#ifndef F_CPU
#define F_CPU 8000000
#endif

#ifndef F_SCL
#define F_SCL 100000
#endif

#include <avr/io.h>
#include <compat/twi.h>

void i2cWait(void)
{
        while(!(TWCR & (1 << TWINT)));
}

void i2cInit(void)
{
        TWSR = 0;
        TWBR = (F_CPU / F_SCL - 16) / 2;
        TWCR = (1 << TWEN);
}

unsigned char i2cStart(unsigned char addr, unsigned char direction)
{
        TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTA);
        i2cWait();
        if(TW_STATUS != TW_START && TW_STATUS != TW_REP_START) return TW_STATUS;
        TWDR = addr + direction;
        TWCR = (1 << TWEN) | (1 << TWINT);
        i2cWait();
        if(TW_STATUS != TW_MT_SLA_ACK && TW_STATUS != TW_MR_SLA_ACK) return 1;
        return 0;
}

unsigned char i2cWrite(unsigned char data)
{
        TWDR = data;
        TWCR = (1 << TWEN) | (1 << TWINT);

        i2cWait();

        if(TW_STATUS != TW_MT_DATA_ACK) return 1;
        return 0;
}

unsigned char i2cRead(unsigned char ack)
{
        if(ack) TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);
        else TWCR = (1 << TWEN) | (1 << TWINT);
        i2cWait();
        return TWDR;
}

void i2cStop(void)
{
        TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTO);

        while(TWCR & (1 << TWSTO));
}

#endif
