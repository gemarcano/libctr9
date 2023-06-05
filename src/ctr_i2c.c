// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright: Gabriel Marcano, 2023

#include <ctr9/ctr_i2c.h>
#include <ctr_core/ctr_core_i2c.h>
#include <stdint.h>

int ctr_i2c_write(ctr_i2c_bus bus, uint8_t device, uint8_t register_, const uint8_t *data, size_t amount)
{
	return ctr_core_i2c_write(bus, device, register_, data, amount);
}

int ctr_i2c_write_one(ctr_i2c_bus bus, uint8_t device, uint8_t register_, uint8_t data)
{
	return ctr_core_i2c_write_one(bus, device, register_, data);
}

int ctr_i2c_read(ctr_i2c_bus bus, uint8_t device, uint8_t register_, uint8_t *data, size_t amount)
{
	return ctr_core_i2c_read(bus, device, register_, data, amount);
}

int ctr_i2c_read_one(ctr_i2c_bus bus, uint8_t device, uint8_t register_, uint8_t *data)
{
	return ctr_core_i2c_read_one(bus, device, register_, data);
}
