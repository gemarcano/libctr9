/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/ctr_hid.h>
#include <stdbool.h>

bool ctr_hid_button_status(ctr_hid_button_type buttons)
{
	return ctr_core_hid_button_status(buttons);
}

ctr_hid_button_type ctr_hid_get_buttons(void)
{
	return ctr_core_hid_get_buttons();
}

//This was taken and adapted from b1l1s's hid.c
void ctr_input_wait(void)
{
	ctr_core_input_wait();
}

