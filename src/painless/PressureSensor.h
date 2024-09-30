//
// Created by iamna on 28/09/2024.
//

#ifndef RP2040_FREERTOS_IRQ_PRESSURESENSOR_H
#define RP2040_FREERTOS_IRQ_PRESSURESENSOR_H


#include <memory>
#include "PicoI2C.h"

class PressureSensor {
public:
    PressureSensor(std::shared_ptr<PicoI2C> i2c, uint16_t device_address);
    int readPressure();

private:
    uint8_t address;
    uint8_t  data[1] = {0xF1};
    uint8_t values[2] = {0};
    uint pressure;

    std::shared_ptr<PicoI2C> pressure_i2c;
};


#endif //RP2040_FREERTOS_IRQ_PRESSURESENSOR_H
