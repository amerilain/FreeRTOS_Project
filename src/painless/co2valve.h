//
// Created by iamna on 29/09/2024.
//

#ifndef RP2040_FREERTOS_IRQ_CO2VALVE_H
#define RP2040_FREERTOS_IRQ_CO2VALVE_H

#include "pico/stdlib.h"
#include "FreeRTOS.h"

class co2valve {
public:
    co2valve(int valve_pin);
    void open();
    void close();
    void setValve(int value);
    int getValve();
private:
    int current_value;
    int previous_value;
    int set_value;

};


#endif //RP2040_FREERTOS_IRQ_CO2VALVE_H
