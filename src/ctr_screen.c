#include <ctr9/ctr_screen.h>
#include <ctr9/i2c.h>

void ctr_screen_enable_backlight(ctr_screen_enum aScreens)
{
	int data = 0;
	if (aScreens & CTR_SCREEN_TOP)
	{
		data |= 1 << 5;
	}

	if (aScreens & CTR_SCREEN_BOTTOM)
	{
		data |= 1 << 3;
	}

	i2cWriteRegister(I2C_DEV_MCU, 0x22, data);
}

void ctr_screen_disable_backlight(ctr_screen_enum aScreens)
{
	int data = 0;
	if (aScreens & CTR_SCREEN_TOP)
	{
		data |= 1 << 4;
	}

	if (aScreens & CTR_SCREEN_BOTTOM)
	{
		data |= 1 << 2;
	}
	i2cWriteRegister(I2C_DEV_MCU, 0x22, data);
}

