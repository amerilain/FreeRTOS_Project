//
// Created by iamna on 01/10/2024.
//
#include <iostream>
#include <sstream>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hardware/gpio.h"
#include "PicoOsUart.h"
#include "ssd1306.h"
#include "pico/stdlib.h"

#include "hardware/timer.h"
extern "C" {
uint32_t read_runtime_ctr(void) {
    return timer_hw->timerawl;
}
}

void tls_task(void *param);
void modbus_task(void *param);
void read_pressur(void *param);
void UI_task(void *param);
void fanSpeedWrite( void *param);
void co2_injector(void *param);


#include "painless/SharedResources.h"

int main()
{
    stdio_init_all();
    printf("\nBoot\n");

    SharedResources sharedResources;

    // UI task
    xTaskCreate(UI_task, "UI", 512, &sharedResources,
                tskIDLE_PRIORITY + 1, nullptr);
    // Network task

    //EEPROM task
    xTaskCreate(tls_task, "tls test", 6000, (void *) nullptr,
                tskIDLE_PRIORITY + 1, nullptr);
    // Pressure sensor Task
    xTaskCreate(read_pressur, "Pressure", 512, &sharedResources,
                tskIDLE_PRIORITY + 1, nullptr);
    //Modbus read task
    xTaskCreate(modbus_task, "Modbus", 512, &sharedResources,
                tskIDLE_PRIORITY + 1, nullptr);
    // Modbus write task
    xTaskCreate(fanSpeedWrite, "FanSpeed", 512, &sharedResources,
                tskIDLE_PRIORITY + 1, nullptr);
    // Co2 injector Task
    xTaskCreate(co2_injector, "Co2Injector", 512, &sharedResources,
                tskIDLE_PRIORITY + 1, nullptr);
    vTaskStartScheduler();

    while(true){};
}

#include <cstdio>
#include "ModbusClient.h"
#include "ModbusRegister.h"

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#if 0
#define UART_NR 0
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#else
#define UART_NR 1
#define UART_TX_PIN 4
#define UART_RX_PIN 5
#endif

#define BAUD_RATE 9600
#define STOP_BITS 2 // for real system (pico simualtor also requires 2 stop bits)


void modbus_task(void *param) {
    auto sharedResources = static_cast<SharedResources *>(param);
    const uint led_pin = 22;
    const uint button = 9;
    const uint co2_injector = 27;

    // Initialize LED pin
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    gpio_init(button);
    gpio_set_dir(button, GPIO_IN);
    gpio_pull_up(button);



    auto uart{std::make_shared<PicoOsUart>(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE, STOP_BITS)};
    auto rtu_client{std::make_shared<ModbusClient>(uart)};
    ModbusRegister co(rtu_client, 240, 256);
    ModbusRegister rh(rtu_client, 241, 256);
    ModbusRegister t(rtu_client, 241, 257);
    ModbusRegister produal(rtu_client, 1, 0);
    produal.write(100);
    vTaskDelay((100));
    produal.write(500);
    int co2 = 0;
    int speed = 90;
    while (true) {
        gpio_put(led_pin, !gpio_get(led_pin)); // toggle  led
        /*co2 = co.read();
        printf("CO=%d\n", co2);
        vTaskDelay(5);
        printf("RH=%5.1f%%\n", rh.read() / 10.0);
        vTaskDelay(5);
        printf("T =%5.1f%%\n", t.read() / 10.0);*/
        co2= co.read();
        sharedResources->setCo2(co2);
        vTaskDelay(5);
        sharedResources->setRH(rh.read());
        vTaskDelay(5);
        sharedResources->setTem(t.read());
        printf("CO2=%d\n", sharedResources->getCo2());
        //print speed
        printf("Speed=%d\n", speed);

        vTaskDelay(500);
    }
}

#include "PicoI2C.h"
#include "painless//PressureSensor.h"
void read_pressur(void *param) {
    /*auto i2cbus{std::make_shared<PicoI2C>(1, 400000)};
    uint8_t data[1] = {0xF1};
    uint8_t values[2] = {0};
    int pressure;
    int sensorValue;
    while (true) {
        i2cbus->transaction(0x40, data, 1, values, 2);
       // i2cbus->read(0x40, values, 2);
        pressure = ( (values[0] << 8) | values[1]) /240 *0.95;  // 0.95 is based on sea level ( based on datasheet)
        pressure = (pressure <= 0) ? 0 : (pressure >= 127) ? 0 : pressure;
        printf("Pressurelast=%d\n", pressure);
        vTaskDelay(3000);

    }*/
    auto sharedResources = static_cast<SharedResources *>(param);
    auto i2cbus{std::make_shared<PicoI2C>(1, 400000)};
    PressureSensor sensor(i2cbus, 0x40);

    while (true) {
        sharedResources->setPressure(sensor.readPressure());
        printf("Pressure=%d\n", sharedResources->getPressure());
        vTaskDelay(100);
    }

}

#include "painless/co2valve.h"
void co2_injector(void *param) {
    auto sharedResources = static_cast<SharedResources *>(param);
    co2valve co2(27);
    while (true) {

        // If level is low then open the valve
        if (sharedResources->getCo2() < sharedResources->getCo2SP()){
            co2.open();
        }
        else if(sharedResources->getCo2() > sharedResources->getCo2SP()){
            co2.close();
        }
        vTaskDelay(1000);
    }

}

// Write task to modbus fan speed

void fanSpeedWrite( void *param){
    auto sharedResources = static_cast<SharedResources *>(param);
    auto uart{std::make_shared<PicoOsUart>(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE, STOP_BITS)};
    auto rtu_client{std::make_shared<ModbusClient>(uart)};
    ModbusRegister produal(rtu_client, 1, 0);
    uint speed = 0;
    while(true){
        // Read the co2 value
        if ( sharedResources->getCo2() > sharedResources->getCo2SP()){
            produal.write(speed < 1000 ? speed+=10 : 1000);
        }
        if( sharedResources->getCo2() < sharedResources->getCo2SP()){
            produal.write(speed <= 0 ? 0 : speed--);
        }
        vTaskDelay(10000);
    }

}

void UI_task(void *param){
    auto sharedResources = static_cast<SharedResources *>(param);

    while(true){
        //printf("CO2=%d\n", sharedResources->getCo2());
        printf("RH=%5.1f%%\n", sharedResources->getRH() / 10.0);
        printf("T =%5.1f%%\n", sharedResources->getTem() / 10.0);
        //printf("Pressure=%d\n", sharedResources->getPressure());

        vTaskDelay(1000);
    }
}

#include "painless/NetworkClass.h"

void tls_task(void *param)
{
    NetworkClass network("Nadim", "nadimahmed");
    bool network_status =false;
    while(true) {
        if (network_status == false)
        {
            network.connect();
            network_status = true;
        }
        if(network_status == true)
        {
            network.recieve();
            vTaskDelay(20000);
        }

        vTaskDelay(100);
    }
}
