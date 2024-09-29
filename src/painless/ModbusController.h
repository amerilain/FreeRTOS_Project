//
// Created by iamna on 28/09/2024.
//

#ifndef RP2040_FREERTOS_IRQ_MODBUSCONTROLLER_H
#define RP2040_FREERTOS_IRQ_MODBUSCONTROLLER_H

#include "PicoOsUart.h"
#include "memory"


#define UART_NR 1
#define UART_TX_PIN 4
#define UART_RX_PIN 5

#define BAUD_RATE 9600
#define STOP_BITS 2

class ModbusController {
public:
    explicit ModbusController(std::shared_ptr<PicoOsUart> uart);
    void writeSensor(const std::string& sensorName, uint16_t value);
    int readSensor(const std::string& sensorName);


};


#endif //RP2040_FREERTOS_IRQ_MODBUSCONTROLLER_H
