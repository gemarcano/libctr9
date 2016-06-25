#include <ctr9/ctr_hid.h>
#include <stdbool.h>

bool ctr_hid_button_status(ctr_hid_button_t buttons)
{
	return ((~(*CTR_HID_REG) & 0xFFF) & buttons) == buttons;
}

ctr_hid_button_t ctr_hid_get_buttons(void)
{
	return ~(*CTR_HID_REG) & 0xFFF;
}

