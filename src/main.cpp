#include <iostream>
#include <sstream>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hardware/gpio.h"
#include "PicoOsUart.h"
#include "ssd1306.h"


#include "hardware/timer.h"
extern "C" {
uint32_t read_runtime_ctr(void) {
    return timer_hw->timerawl;
}
}


void modbus_task(void *param);
void read_pressur(void *param);

extern "C" {
    void tls_test(void);
}
void tls_task(void *param)
{
    tls_test();
    while(true) {
        vTaskDelay(100);
    }
}

#include "pico/stdlib.h"
int main()
{
    stdio_init_all();
    printf("\nBoot\n");


    xTaskCreate(modbus_task, "Modbus", 512, (void *) nullptr,
                tskIDLE_PRIORITY + 1, nullptr);
    xTaskCreate(read_pressur, "Pressure", 512, (void *) nullptr,
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

#define USE_MODBUS

void modbus_task(void *param) {

    const uint led_pin = 22;
    const uint button = 9;
    const uint co2_injector = 27;

    // Initialize LED pin
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    gpio_init(button);
    gpio_set_dir(button, GPIO_IN);
    gpio_pull_up(button);

    gpio_init(co2_injector);
    gpio_set_dir(co2_injector, GPIO_OUT);
    gpio_pull_up(co2_injector);

    // Initialize chosen serial port
    //stdio_init_all();

    //printf("\nBoot\n");

#ifdef USE_MODBUS
    auto uart{std::make_shared<PicoOsUart>(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE, STOP_BITS)};
    auto rtu_client{std::make_shared<ModbusClient>(uart)};
    ModbusRegister co(rtu_client, 240, 256);
    ModbusRegister rh(rtu_client, 241, 256);
    ModbusRegister t(rtu_client, 241, 257);
    ModbusRegister produal(rtu_client, 1, 0);
    produal.write(100);
    vTaskDelay((100));
    produal.write(800);
#endif
    int co2 = 0;

    while (true) {
#ifdef USE_MODBUS
        gpio_put(led_pin, !gpio_get(led_pin)); // toggle  led
        co2 = co.read();
        printf("CO=%d\n", co2);
        vTaskDelay(5);
        printf("RH=%5.1f%%\n", rh.read() / 10.0);
        vTaskDelay(5);
        printf("T =%5.1f%%\n", t.read() / 10.0);
        if(co2 < 500){
            gpio_put(co2_injector, 1);
        }
        else if(co2 > 500){
            gpio_put(co2_injector, 0);
        }
        vTaskDelay(3000);
#endif
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

    auto i2cbus{std::make_shared<PicoI2C>(1, 400000)};
    PressureSensor sensor(i2cbus, 0x40);

    while (true) {
        printf("Pressure=%d\n", sensor.readPressure());
        vTaskDelay(3000);
    }

}
