#ifndef RP2040_FREERTOS_IRQ_CO2VALVE_H
#define RP2040_FREERTOS_IRQ_CO2VALVE_H

#include "pico/stdlib.h"
#include "FreeRTOS.h"

class co2valve {
public:
    co2valve(int valve_pin);
    void open();
    void close();
private:
    uint valve_pin;
};


#endif //RP2040_FREERTOS_IRQ_CO2VALVE_H
