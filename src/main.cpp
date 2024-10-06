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
#include "semphr.h"
#include "hardware/timer.h"

#include "painless/SharedResources.h"
#include <cstdio>
#include "ModbusClient.h"
#include "ModbusRegister.h"
#include "PicoI2C.h"
#include "painless//PressureSensor.h"

#include "painless/co2valve.h"
#include"painless/Menu/Menu.h"
#include "painless/NetworkClass.h"
#include "eeprom/EEPROM.h"

#include "Button_Rotary_irq/Interrupt_Handler.h"
#include "queue.h"

extern "C" {
uint32_t read_runtime_ctr(void) {
    return timer_hw->timerawl;
}
}

SemaphoreHandle_t mutex;
SemaphoreHandle_t i2cmutex;

void NetworkTask(void *param);
void modbus_task(void *param);
void read_pressur(void *param);
void UI_task(void *param);
void fanSpeedWrite( void *param);
void co2_injector(void *param);
void eeprom_task(void *param);
void InterruptHandler(void *param);




int main()
{
    stdio_init_all();
    printf("\nBoot\n");

   SharedResources sharedResourcesPtr;
    //auto sharedResourcesPtr = std::make_shared<SharedResources>();
    mutex = xSemaphoreCreateMutex();

    // Network task
    xTaskCreate(NetworkTask, "NetworkTask", 6000, (void *) nullptr,
                tskIDLE_PRIORITY + 1, nullptr);
   /* //EEPROM task
    xTaskCreate(eeprom_task, "EEPROM", 512, &sharedResourcesPtr,
                tskIDLE_PRIORITY + 1, nullptr);*/
    // UI task
    xTaskCreate(UI_task, "UI", 512, &sharedResourcesPtr,
                tskIDLE_PRIORITY + 1, nullptr);
/*
    // Pressure sensor Task
    xTaskCreate(read_pressur, "Pressure", 512, &sharedResourcesPtr,
                tskIDLE_PRIORITY + 1, nullptr);
    //Modbus read task
    xTaskCreate(modbus_task, "Modbus", 512, &sharedResourcesPtr,
                tskIDLE_PRIORITY + 1, nullptr);
    // Modbus write task
    *//*xTaskCreate(fanSpeedWrite, "FanSpeed", 512, &sharedResourcesPtr,
                tskIDLE_PRIORITY + 1, nullptr);*//*
    // Co2 injector Task
    xTaskCreate(co2_injector, "Co2Injector", 512, &sharedResourcesPtr,
                tskIDLE_PRIORITY + 1, nullptr);*/

    //Interrupthandler rotary(ROT_A_PIN, ROT_B_PIN, ROT_SW_PIN & button 0, button 1, button 2);
    xTaskCreate(InterruptHandler, "InterruptHandler", 512, &sharedResourcesPtr,
                tskIDLE_PRIORITY + 1, nullptr);
    // Start the scheduler
    vTaskStartScheduler();
    while(true){};
}



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

   //uto uart{std::make_shared<PicoOsUart>(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE, STOP_BITS)};
  //auto rtu_client{std::make_shared<ModbusClient>(uart)};
    auto rtu_client{std::make_shared<ModbusClient>(sharedResources->uart420)};
    ModbusRegister co(rtu_client, 240, 256);
    ModbusRegister rh(rtu_client, 241, 256);
    ModbusRegister t(rtu_client, 241, 257);

    vTaskDelay((100));

    int co2 = 0;

    while (true) {
        if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE){
            gpio_put(led_pin, !gpio_get(led_pin)); // toggle  led
            co2= co.read();
            sharedResources->setCo2(co2);
            vTaskDelay(5);
            sharedResources->setRH(rh.read());
            vTaskDelay(5);
            sharedResources->setTem(t.read());
           // printf("CO2=%d\n", sharedResources->getCo2());
            xSemaphoreGive(mutex);
        }
        vTaskDelay(500);
    }
}

void read_pressur(void *param) {
    auto sharedResources = static_cast<SharedResources *>(param);
    //auto i2cbus{std::make_shared<PicoI2C>(1, 400000)};
    PressureSensor sensor(sharedResources->i2cbus, 0x40);

    while (true) {
        sharedResources->setPressure(sensor.readPressure());
        //printf("Pressure=%d\n", sharedResources->getPressure());
        vTaskDelay(100);
    }

}


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
    //to uart{std::make_shared<PicoOsUart>(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE, STOP_BITS)};
    auto rtu_client{std::make_shared<ModbusClient>(sharedResources->uart420)};
    ModbusRegister produal(rtu_client, 1, 0);
    uint speed = 0;
    while(true){
        if(xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE){
            if ( sharedResources->getCo2() > sharedResources->getCo2SP()){
                produal.write(speed < 200 ? speed+=10 : 200);
            }
            if( sharedResources->getCo2() < sharedResources->getCo2SP()){
                produal.write(speed <= 0 ? 0 : speed--);
            }
            sharedResources->setFanSpeed(speed);
            /*printf("Speed=%d\n", speed);
            printf("Shemaphore taken\n");*/
            xSemaphoreGive(mutex);
        }
       /* if ( sharedResources->getCo2() > sharedResources->getCo2SP()){
            produal.write(speed < 1000 ? speed+=10 : 1000);
        }
        if( sharedResources->getCo2() < sharedResources->getCo2SP()){
            produal.write(speed <= 0 ? 0 : speed--);
        }*/
        vTaskDelay(100);
    }

}


void UI_task(void *param){
    auto sharedResourcesPtr = static_cast<SharedResources *>(param);
    //auto sharedResourcesPtr = std::make_shared<SharedResources>();
    auto display {std::make_shared<TFTDisplay>(sharedResourcesPtr->i2cbus)};
    Menu menu(display , static_cast<std::shared_ptr<SharedResources>>(sharedResourcesPtr));
    SharedResources::EventType event;
    display->mainMenu();
    while(true){
      /*  if (xQueueReceive(sharedResources->xbuttonQueue, &event, portMAX_DELAY) == pdTRUE){
           menu.event(event);
        }*/
      menu.event(sharedResourcesPtr->Event);
      //printf("MENU sHOW\n");
      if (sharedResourcesPtr->isRotaryClockwise){
          printf("Rotary Clockwise\n");
        }

        vTaskDelay(100);
    }
}



void NetworkTask(void *param)
{
    auto sharedResources = static_cast<SharedResources *>(param);
    NetworkClass network;
    bool network_status =false;
    bool transmit = false;
    while(true) {
        // print SSID and password
        printf("SSID=%s\n", sharedResources->getSSID());
        printf("Password=%s\n", sharedResources->getPassword());
        if (sharedResources->credentialsEntered){

            network.setCredentials(sharedResources->getSSID(), sharedResources->getPassword());
            printf("I am here");
            sharedResources->credentialsEntered = false;
            network_status = true;
        }
        if (network_status)
        {
            network.connect();
            transmit = true;
            network_status = false;
        }
        if(transmit)
        {
            network.recieve();
           /* if (network.Co2_SetPoint != sharedResources->getCo2SP() && network.Co2_SetPoint != 0) {
                sharedResources->setCo2SP(network.Co2_SetPoint);
                printf("CO2 Set Point in main=%d\n", sharedResources->getCo2SP());
            }*/
            vTaskDelay(10);
        }

        vTaskDelay(100);
    }
}

void eeprom_task(void *param){

    auto sharedResources = static_cast<SharedResources *>(param);
    auto i2cbus{std::make_shared<PicoI2C>(1, 400000)};
    EEPROM eeprom (i2cbus, 0x50);
    const char *SSID = "Nadim";
    const char *PASS = "nadimahmed";
    eeprom.writeToMemory(0, (uint8_t*)SSID, sizeof(sharedResources->getCo2SP()));
    eeprom.writeToMemory(64, (uint8_t*)PASS, sizeof(sharedResources->getCo2SP()));
    eeprom.writeToMemory(128, (uint8_t*)sharedResources->getCo2SP(), sizeof(sharedResources->getCo2SP()));
    vTaskDelay(1000);


    while(true){
       /* if (xSemaphoreTake(i2cmutex, portMAX_DELAY) == pdTRUE){
            eeprom.writeToMemory(0, (uint8_t*)SSID, sizeof(sharedResources->getCo2SP()));
            eeprom.writeToMemory(64, (uint8_t*)PASS, sizeof(sharedResources->getCo2SP()));
            eeprom.writeToMemory(128, (uint8_t*)sharedResources->getCo2SP(), sizeof(sharedResources->getCo2SP()));
            xSemaphoreGive(mutex);
        }*/

        vTaskDelay(1000);
    }

}


void InterruptHandler(void *param){
    auto sharedResources = static_cast<SharedResources *>(param);
    uint8_t rotA = 10;
    uint8_t rotB = 11;
    uint8_t rotP = 12;
    uint8_t SW0 = 9;
    uint8_t SW1 = 8;
    uint8_t SW2 = 7;
    Interrupt_Handler rothandlerA(rotA);
    Interrupt_Handler rothandlerP(rotP);
    Interrupt_Handler sw0(SW0);
    Interrupt_Handler sw1(SW1);
    Interrupt_Handler sw2(SW2);
    SharedResources::EventType event;

    while(true){

        if(rothandlerA.rotaryturned){
            if(rothandlerA.count == 1){
                sharedResources->isRotaryClockwise = true;
            }
            else if(rothandlerA.count == -1){
                sharedResources->isRotaryCounterClockwise = true;
            }
            rothandlerA.rotaryturned = false;
        }
        if (rothandlerP.buttonPressed){
            sharedResources->isRotaryPressed = true;
            rothandlerP.buttonPressed = false;
        }
        if (sw0.buttonPressed){
            sharedResources->isSW0Pressed = true;
            sw0.buttonPressed = false;
        }
        if (sw1.buttonPressed){
            sharedResources->isSW1Pressed = true;
            sw1.buttonPressed = false;
        }
        if (sw2.buttonPressed){
            sharedResources->isSW2Pressed = true;
            sw2.buttonPressed = false;
        }


        vTaskDelay(10);
    }

}