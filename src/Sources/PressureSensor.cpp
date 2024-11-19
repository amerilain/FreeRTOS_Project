#include "PressureSensor.h"

PressureSensor::PressureSensor(std::shared_ptr<PicoI2C> i2c, uint16_t device_address): pressure_i2c(i2c), address(device_address) {
    pressure = 0;

}

int PressureSensor::readPressure() {
    pressure_i2c->transaction(address, data, 1, values, 2);
    pressure = ( (values[0] << 8) | values[1]) /240 *0.95;  // 0.95 is based on sea level ( based on datasheet)
    pressure = (pressure <= 0) ? 0 : (pressure >= 150) ? 0 : pressure;
    return pressure;
}