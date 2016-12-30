#include <ctr9/i2c.h>
#include <stdint.h>

void i2cStop(uint8_t bus_id, uint8_t arg0) {
    ctr_core_i2cStop(bus_id, arg0);
}

//-----------------------------------------------------------------------------

bool i2cSelectDevice(uint8_t bus_id, uint8_t dev_reg) {
    return ctr_core_i2cSelectDevice(bus_id, dev_reg);
}

bool i2cSelectRegister(uint8_t bus_id, uint8_t reg) {
    return ctr_core_i2cSelectRegister(bus_id, reg);
}

//-----------------------------------------------------------------------------

uint8_t i2cReadRegister(uint8_t dev_id, uint8_t reg) {
    return ctr_core_i2cReadRegister(dev_id, reg);
}

bool i2cReadRegisterBuffer(uint8_t dev_id, uint8_t reg, uint8_t* buffer, size_t buf_size) {
    return ctr_core_i2cReadRegisterBuffer(dev_id, reg, buffer, buf_size);
}

bool i2cWriteRegister(uint8_t dev_id, uint8_t reg, uint8_t data) {
    return ctr_core_i2cWriteRegister(dev_id, reg, data);
}

bool i2cWriteRegisterBuffer(uint8_t dev_id, uint8_t reg, const uint8_t *buffer, size_t buf_size) {
    return ctr_core_i2cWriteRegisterBuffer(dev_id, reg, buffer, buf_size);
}

