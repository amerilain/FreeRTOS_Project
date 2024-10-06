//
// Created by iamna on 02/10/2024.
//

#ifndef RP2040_FREERTOS_IRQ_SHAREDRESOURCES_H
#define RP2040_FREERTOS_IRQ_SHAREDRESOURCES_H

#include <memory>
#include "uart//PicoOsUart.h"
#include "i2c/PicoI2C.h"

#include "queue.h"


#define UART_NR 1
#define UART_TX_PIN 4
#define UART_RX_PIN 5

class SharedResources {
public:

    SharedResources();
    int getCo2();
    void setCo2(int co2);

    int getTem();
    void setTem(int tem);

    int getRH();
    void setRH(int RH);

    int getFanSpeed();
    void setFanSpeed(int fanSpeed);

    int getCo2SP();
    void setCo2SP(int co2SP);

    int getPressure();
    void setPressure(int pressure);

    char* getSSID();
    void setSSID(char* ssid);

    char* getPassword();
    void setPassword(char* password);


    QueueHandle_t xbuttonQueue;


    std::shared_ptr<PicoOsUart> uart420= std::make_shared<PicoOsUart>(UART_NR, UART_TX_PIN, UART_RX_PIN, 9600,  2);
    // Add i2c shared resources here
    std::shared_ptr<PicoI2C> i2cbus = std::make_shared<PicoI2C>(1, 400000);

    enum EventType {
        ROT_CLOCKWISE,
        ROT_COUNTER_CLOCKWISE,
        ROT_PRESSED,
        SW0_PRESSED,
        SW1_PRESSED,
        SW2_PRESSED,
        Nothing
    };
    EventType Event;

    bool isRotaryClockwise = false;
    bool isRotaryCounterClockwise = false;
    bool isRotaryPressed = false;
    bool isSW0Pressed = false;
    bool isSW1Pressed = false;
    bool isSW2Pressed = false;

    bool credentialsEntered = false;


private:
    int co2;
    int tem;
    int RH;
    int pressure;
    int fanSpeed;
    int co2SP;

    char *ssid2;
    char *password2;

    // Add uart shareted resources here

    // Add modbus shared resources here
    // Add i2c shared resources here


};


#endif //RP2040_FREERTOS_IRQ_SHAREDRESOURCES_H
