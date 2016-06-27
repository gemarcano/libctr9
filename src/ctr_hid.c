#include <ctr9/ctr_hid.h>
#include <stdbool.h>

bool ctr_hid_button_status(ctr_hid_button_type buttons)
{
	return (~(*CTR_HID_REG) & 0xFFF) == buttons;
}

ctr_hid_button_type ctr_hid_get_buttons(void)
{
	return ~(*CTR_HID_REG) & 0xFFF;
}

