#include <ctr9/i2c.h>
#include <time.h>

void ctr_rtc_init()
{

}

void ctr_rtc_gettime()
{
	uint8_t data[8] = {0};

	i2cReadRegisterBuffer(I2C_DEV_MCU, 0x30, data, 8);
}

