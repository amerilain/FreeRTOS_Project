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

#include "Sources/SharedResources.h"
#include <cstdio>
#include "ModbusClient.h"
#include "ModbusRegister.h"
#include "PicoI2C.h"
#include "Sources/PressureSensor.h"

#include "Sources/co2valve.h"
#include"Menu/Menu.h"
#include "Sources/NetworkClass.h"
#include "eeprom/EEPROM.h"

#include "queue.h"

#include "Sources//irqs.h"

#define ROT_A_PIN 10
#define ROT_B_PIN 11
#define ROT_SW_PIN 12
#define BUTTON_0 9
#define BUTTON_1 8
#define BUTTON_2 7

#define SSID_ADDR 0
#define PASSWORD_ADDR 64
#define CO2SP_ADDR 128
#define READ_WRITE_SIZE 62

#define LOWEST_CO2SP 200
#define HIGHEST_CO2SP 1500

#define RISK_POINT 2000
#define FAN_SPEED_MAX 1000
#define FAN_SPEED_MIN 0

#define produal_ServerADDR 1
#define produal_RegisterADDR 0

#define LED_PIN 22
#define CoServerADDR 240
#define CoRegisterADDR 256
#define RhServerADDR 241
#define RhRegisterADDR 256
#define TServerADDR 241
#define TRegisterADDR 257


#define CO2_INJECTOR 27
#define WaitingTime 60000
#define ValveOpenTime 2000

#define PressureSensorAdd 0x40

#define EVENT_READ_PRESSURE     (1 << 0)

#define EVENT_SSID_WRITE     (1 << 0)
#define EVENT_SSID_READ (1 << 1)
#define EVENT_NETWORK_CONNECTED (1 << 2)


#include "hardware/timer.h"
/**
 * @brief Reads the runtime counter.
 *
 * This function reads the current value of the runtime counter from the hardware timer.
 *
 * @return uint32_t The current value of the runtime counter.
 */
extern "C" {
uint32_t read_runtime_ctr(void) {
    return timer_hw->timerawl;
}
}

void NetworkTask(void *param);
void modbus_task(void *param);
void read_pressure(void *param);
void UI_task(void *param);
void fanSpeedWrite( void *param);
void co2_injector(void *param);
void eeprom_task(void *param);
void InterruptHandler(void *param);

/**
 * @brief Main function to initialize and start tasks.
 *
 * This function initializes the standard I/O, prints a boot message, and creates
 * various tasks for network operations, EEPROM operations, UI handling, pressure
 * sensor reading, Modbus communication, fan speed control, CO2 injection, and
 * interrupt handling. It then starts the FreeRTOS scheduler to run these tasks.
 *
 * @return int This function does not return.
 */

int main()
{
    stdio_init_all();
    printf("\nBoot\n");

   SharedResources sharedResourcesPtr;
    //auto sharedResourcesPtr = std::make_shared<SharedResources>();
    sharedResourcesPtr.mutex = xSemaphoreCreateMutex();

    // Network task
    xTaskCreate(NetworkTask, "NetworkTask", 6000, &sharedResourcesPtr,
                tskIDLE_PRIORITY + 1, nullptr);
    //EEPROM task
    xTaskCreate(eeprom_task, "EEPROM", 512, &sharedResourcesPtr,
                tskIDLE_PRIORITY + 1, nullptr);
    // UI task
    xTaskCreate(UI_task, "UI", 512, &sharedResourcesPtr,
                tskIDLE_PRIORITY + 1, nullptr);
    // Pressure sensor Task
    xTaskCreate(read_pressure, "Pressure", 512, &sharedResourcesPtr,
                tskIDLE_PRIORITY + 1, nullptr);
    //Modbus read task
    xTaskCreate(modbus_task, "Modbus", 512, &sharedResourcesPtr,
                tskIDLE_PRIORITY + 1, nullptr);
    // Modbus write task
    xTaskCreate(fanSpeedWrite, "FanSpeed", 512, &sharedResourcesPtr,
                tskIDLE_PRIORITY + 1, nullptr);
    // Co2 injector Task
    xTaskCreate(co2_injector, "Co2Injector", 512, &sharedResourcesPtr,
                tskIDLE_PRIORITY + 1, nullptr);

    //Interrupthandler rotary(ROT_A_PIN, ROT_B_PIN, ROT_SW_PIN & button 0, button 1, button 2);
    xTaskCreate(InterruptHandler, "InterruptHandler", 512, &sharedResourcesPtr,
                tskIDLE_PRIORITY + 1, nullptr);
    // Start the scheduler
    vTaskStartScheduler();
    while(true){};
}

/**
 * @brief Task to handle Modbus operations.
 *
 * This task manages Modbus communication by reading data from Modbus registers
 * and updating the shared resources. It also toggles an LED pin to indicate activity.
 * The task runs indefinitely with a fixed delay between iterations.
 *
 * @param param Pointer to shared resources.
 */

void modbus_task(void *param) {
    auto sharedResources = static_cast<SharedResources *>(param);

    // Initialize LED pin
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    auto rtu_client{std::make_shared<ModbusClient>(sharedResources->uart420)};

    // Initialize modbus registers
    ModbusRegister co(rtu_client, CoServerADDR, CoRegisterADDR);
    ModbusRegister rh(rtu_client, RhServerADDR, RhRegisterADDR);
    ModbusRegister t(rtu_client, TServerADDR, TRegisterADDR);


    while (true) {
        if (xSemaphoreTake(sharedResources->mutex, portMAX_DELAY) == pdTRUE){
            gpio_put(LED_PIN, !gpio_get(LED_PIN)); // toggle  led
            sharedResources->setCo2(co.read());
            vTaskDelay(pdMS_TO_TICKS(5));
            sharedResources->setRH(rh.read());
            vTaskDelay(pdMS_TO_TICKS(5));
            sharedResources->setTem(t.read());
            vTaskDelay(pdMS_TO_TICKS(5));
            xSemaphoreGive(sharedResources->mutex);
        }
        vTaskDelay(100);
    }
}

/**
 * @brief Task to read pressure from the pressure sensor.
 *
 * This task continuously reads the pressure from the pressure sensor and updates
 * the shared resources with the latest pressure value. The task runs indefinitely
 * with a fixed delay between iterations. The pressure reading is triggered by an event.
 *
 * @param param Pointer to shared resources.
 */

void read_pressure(void *param) {
    auto sharedResources = static_cast<SharedResources *>(param);
    PressureSensor sensor(sharedResources->i2cbus, PressureSensorAdd);

    while (true) {
        // Dose not clear bits after reading the pressure , Cleared in fan speed write task after stop the fan
        if(xEventGroupGetBits(sharedResources->eventGroup) & EVENT_READ_PRESSURE){
            sharedResources->setPressure(sensor.readPressure());
        }
        vTaskDelay(10);
    }
}

/**
 * @brief Task to control the CO2 injector based on CO2 levels.
 *
 * This task reads the CO2 levels and compares them with the setpoint.
 * If the CO2 level is below the setpoint, the CO2 valve is opened for a specified duration.
 * The task runs periodically with a fixed delay.
 *
 * @param param Pointer to shared resources.
 */

void co2_injector(void *param) {
    auto sharedResources = static_cast<SharedResources *>(param);
    co2valve co2(CO2_INJECTOR);
    int Co2 = 0;
    int Co2SP = 0;
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(WaitingTime);
    while (true) {
        // wait for 1 minute
        vTaskDelayUntil(&lastWakeTime, xFrequency);
        Co2 = sharedResources->getCo2();
        Co2SP = sharedResources->getCo2SP();
        // If level is low then open the valve
        if (Co2 < Co2SP) {
            co2.open();
            vTaskDelay(pdMS_TO_TICKS(ValveOpenTime));
            co2.close();
        }

    }
}

/**
 * @brief Task to control the fan speed based on CO2 levels.
 *
 * This task reads the CO2 levels and adjusts the fan speed accordingly.
 * If the CO2 level exceeds a predefined risk point, the fan is started at maximum speed and set the event bit for pressure read task .
 * Once the CO2 level drops below the setpoint, the fan is stopped and clears the event bit to  restrict the read pressure task.
 *
 * @param param Pointer to shared resources.
 */

void fanSpeedWrite( void *param){
    auto sharedResources = static_cast<SharedResources *>(param);
    auto rtu_client{std::make_shared<ModbusClient>(sharedResources->uart420)};
    ModbusRegister produal(rtu_client, produal_ServerADDR, produal_RegisterADDR);
    uint speed = 0;
    bool startTOincrease =false;
    bool start = false;
    int co2 = 0;
    int co2SP = 0;
    while(true){
        if(xSemaphoreTake(sharedResources->mutex, portMAX_DELAY) == pdTRUE){
            co2 = sharedResources->getCo2();
            co2SP = sharedResources->getCo2SP();
            if (co2 > RISK_POINT){
                startTOincrease = true;
            }
            if(startTOincrease) {
                // start the fan
                if (!start) {
                    speed = FAN_SPEED_MAX;
                    produal.write(speed);
                    // Set The Bits to start reading Pressure sensor
                    xEventGroupSetBits(sharedResources->fanControllerEvent, EVENT_READ_PRESSURE);
                    start = true;
                }
                // stop the fan
                else if(co2 < co2SP){
                    startTOincrease = false;
                    start = false; // prepare fan to start for next time
                    speed = FAN_SPEED_MIN;
                    produal.write(speed);
                    // Clear the bits to stop reading Pressure sensor
                    xEventGroupClearBits(sharedResources->fanControllerEvent, EVENT_READ_PRESSURE);
                }
            }
            sharedResources->setFanSpeed(speed);
            xSemaphoreGive(sharedResources->mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }

}

/**
 * @brief Task to handle the User Interface (UI) operations.
 *
 * This task initializes the display and menu, and continuously handles menu events.
 * It runs indefinitely with a fixed delay between iterations.
 *
 * @param param Pointer to shared resources.
 */

void UI_task(void *param){
    auto sharedResourcesPtr = static_cast<SharedResources *>(param);
    //auto sharedResourcesPtr = std::make_shared<SharedResources>();
    auto display {std::make_shared<TFTDisplay>(sharedResourcesPtr->i2cbus)};
    Menu menu(display , static_cast<std::shared_ptr<SharedResources>>(sharedResourcesPtr));
    display->mainMenu();
    while(true){
      menu.event();
      vTaskDelay(100);
    }
}


/**
 * @brief Task to handle network operations.
 *
 * This task manages network connectivity and data transmission.
 * It sets network credentials, connects to the network, and sends/receives data.
 * The task runs indefinitely with a fixed delay between iterations.
 *
 * @param param Pointer to shared resources.
 */

void NetworkTask(void *param) {
    auto sharedResources = static_cast<SharedResources *>(param);
    NetworkClass network(static_cast<std::shared_ptr<SharedResources>>(sharedResources));
    bool network_status = false;
    bool transmit = false;
    while (true) {
        EventBits_t uxBits = xEventGroupGetBits(sharedResources->eventGroup);
        if (uxBits & EVENT_SSID_READ || sharedResources->credentialsEntered) {
            network.setCredentials(sharedResources->getSSID(), sharedResources->getPassword());

            // Only for demo set the ssid and password Manually

            /*sharedResources->setSSID("SmartIotMQTT");
            sharedResources->setPassword("SmartIot");
            network.setCredentials("SmartIotMQTT", "SmartIot");*/

            // Handle SSID read
            sharedResources->credentialsEntered = false;
            xEventGroupClearBits(sharedResources->eventGroup, EVENT_SSID_READ);
            xEventGroupSetBits(sharedResources->eventGroup, EVENT_SSID_WRITE );
            network_status = true;

        }
        if (network_status) {
            network.connect();
            transmit = true;
            network.transmit = true;
            network_status = false;
        }
        // if the network is connected then send the data and receive the command
        if (transmit) {
          network.sendAndreceive(sharedResources->getCo2(), sharedResources->getTem()/10, sharedResources->getRH()/10,
                             sharedResources->getFanSpeed()/10, sharedResources->getCo2SP());
            if (network.Co2_SetPoint != sharedResources->getCo2SP() && network.Co2_SetPoint >= LOWEST_CO2SP &&
                network.Co2_SetPoint <= HIGHEST_CO2SP) {
                sharedResources->setCo2SP(network.Co2_SetPoint);
            }

        }
        vTaskDelay(10);
    }
}

/**
 * @brief Task to handle EEPROM operations.
 *
 * This task manages reading from and writing to the EEPROM.
 * It writes SSID and password to the EEPROM when they change or are set .
 * It reads SSID, password, and CO2 setpoint from the EEPROM after every boot.
 * It also writes the CO2 setpoint to the EEPROM if it changes.
 *
 * @param param Pointer to shared resources.
 */

void eeprom_task(void *param){

    auto sharedResources = static_cast<SharedResources *>(param);
    auto i2cbus{std::make_shared<PicoI2C>(0, 100000)};
    EEPROM eeprom (i2cbus, 0x50);
    char readbuffer[64] = {0};
    char ssidbuffer[64] = {0};
    char passbuffer[64] = {0};
    bool boot = true;
    std::string CO2SP;
    int oldCO2SP = 0;
    int newCO2SP = 0;

    while(true){
        EventBits_t uxBits = xEventGroupGetBits(sharedResources->eventGroup);
        // write to the eeprom if the ssid and password changes or Set first time
        if (uxBits & EVENT_SSID_WRITE) {
            eeprom.writeToMemory(SSID_ADDR, (uint8_t*)sharedResources->getSSID(), READ_WRITE_SIZE);
            eeprom.writeToMemory(PASSWORD_ADDR, (uint8_t*)sharedResources->getPassword(), READ_WRITE_SIZE);
            xEventGroupClearBits(sharedResources->eventGroup, EVENT_SSID_WRITE);
        }

       // read from the eeprom after every boot
        if(boot){
            eeprom.readFromMemory(SSID_ADDR, (uint8_t*)ssidbuffer, READ_WRITE_SIZE);
           eeprom.readFromMemory(PASSWORD_ADDR, (uint8_t*)passbuffer, READ_WRITE_SIZE);
            if(ssidbuffer != NULL && passbuffer != NULL){
                sharedResources->setSSID(ssidbuffer);
                sharedResources->setPassword(passbuffer);
              xEventGroupSetBits(sharedResources->eventGroup, EVENT_SSID_READ);
               // sharedResources->credentialsEntered = true;
            }
            eeprom.readFromMemory(CO2SP_ADDR, (uint8_t*)readbuffer, READ_WRITE_SIZE);
            sharedResources->setCo2SP(atoi(readbuffer));
            boot = false;
        }

        // write to the eeprom if the set point changes
        if(oldCO2SP != sharedResources->getCo2SP()){
            newCO2SP = sharedResources->getCo2SP();
            CO2SP = std::to_string(newCO2SP);
            const char* numberChar = CO2SP.c_str();
            eeprom.writeToMemory(CO2SP_ADDR, (uint8_t*)numberChar, READ_WRITE_SIZE);
            oldCO2SP = newCO2SP;
        }

        vTaskDelay(100);
    }

}

/**
 * @brief Task to handle interrupt events.
 *
 * This task initializes interrupt handlers for rotary encoder and buttons.
 * It continuously checks the event queue for any interrupt events and updates
 * the shared resources accordingly. The task runs indefinitely with a fixed delay
 * between iterations.
 *
 * @param param Pointer to shared resources.
 */

void InterruptHandler(void *param){
    auto sharedResources = static_cast<SharedResources *>(param);
    irqs rothandlerA(ROT_A_PIN);
   // irqs rothandlerB(rotB);
    irqs rothandlerP(ROT_SW_PIN);
    irqs sw0(BUTTON_0);
    irqs sw1(BUTTON_1);
    irqs sw2(BUTTON_2);


    while(true){

        // Queue  receive the events
        if (xQueueReceive(rothandlerA.eventQueue, &rothandlerA.event, portMAX_DELAY) == pdTRUE){
            switch (rothandlerA.event) {
                case irqs::ROTARY_ENCODER_CW_EVENT:
                    sharedResources->isRotaryClockwise = true;
                    break;
                case irqs::ROTARY_ENCODER_CCW_EVENT:
                    sharedResources->isRotaryCounterClockwise = true;
                    break;
                case irqs::BUTTON_PRESS_EVENT:
                    sharedResources->isRotaryPressed = true;
                    break;
                case irqs::BUTTON_PRESS_EVENT_0:
                    sharedResources->isSW0Pressed = true;
                    break;
                case irqs::BUTTON_PRESS_EVENT_1:
                    sharedResources->isSW1Pressed = true;
                    break;
                case irqs::BUTTON_PRESS_EVENT_2:
                    sharedResources->isSW2Pressed = true;
                    break;
                default:
                    break;
            }
        }
        vTaskDelay(10);
    }

}

