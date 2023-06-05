// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright: Gabriel Marcano, 2023

#ifndef CTR_I2C_H_
#define CTR_I2C_H_

#include <ctr_core/ctr_core_i2c.h>

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef ctr_core_i2c_bus ctr_i2c_bus;

int ctr_i2c_write(ctr_i2c_bus bus, uint8_t device, uint8_t register_, const uint8_t *data, size_t amount);
int ctr_i2c_write_one(ctr_i2c_bus bus, uint8_t device, uint8_t register_, uint8_t data);
int ctr_i2c_read(ctr_i2c_bus bus, uint8_t device, uint8_t register_, uint8_t *data, size_t amount);
int ctr_i2c_read_one(ctr_i2c_bus bus, uint8_t device, uint8_t register_, uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif//CTR_I2C_H_
