#include <iostream>
#include <sstream>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hardware/gpio.h"
#include "PicoOsUart.h"
#include "ssd1306.h"
#include "eeprom/EEPROM.h"
#include "tasks/EepromTask.cpp"
#include "PicoI2C.h"
#include "pico/stdlib.h"
#include <memory>  // Include shared_ptr
#include <cstring>

#include "hardware/timer.h"
extern "C" {
uint32_t read_runtime_ctr(void) {
    return timer_hw->timerawl;
}
}

extern EEPROM eeprom;
bool settingsUpdated = false;

// Define the I2C mutex here in main.cpp
SemaphoreHandle_t i2c_mutex;

void EepromTask(void *params) {
    while (true) {
        if (settingsUpdated) {
            uint8_t co2_setpoint = 5; // random value for now....
            eeprom.writeToMemory(0x00, co2_setpoint);
            settingsUpdated = false;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void i2c_task(void *param) {
    // Ensure mutex is created successfully
    i2c_mutex = xSemaphoreCreateMutex();
    if (i2c_mutex == NULL) {
        printf("Failed to create I2C mutex!\n");
        vTaskDelete(NULL); // Stop the task if mutex creation failed
        return;
    }

    auto i2cbus{std::make_shared<PicoI2C>(0, 100000)};
    EEPROM eeprom(i2cbus, 0x50);

    const uint led_pin = 21;
    const uint delay = pdMS_TO_TICKS(250);
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    // The string to write to EEPROM
    const char* write_string = "Hello, World!";
    size_t string_length = strlen(write_string);

    // Write the string to EEPROM
    eeprom.writeToMemory(0x00, (const uint8_t*)write_string, string_length);
    printf("Written '%s' to EEPROM.\n", write_string);

    // Buffer to hold the read string
    char read_buffer[64] = {0};  // Make sure this buffer is large enough

    // Read the string back from EEPROM
    eeprom.readFromMemory(0x00, (uint8_t*)read_buffer, string_length);
    printf("Read '%s' from EEPROM.\n", read_buffer);

    while (true) {
        gpio_put(led_pin, 1);
        vTaskDelay(delay);
        gpio_put(led_pin, 0);
        vTaskDelay(delay);
    }
}

int main() {
    stdio_init_all();

    // Create the I2C task
    xTaskCreate(i2c_task, "i2c_task", 256, NULL, 1, NULL);

    // Start FreeRTOS scheduler
    vTaskStartScheduler();

    while (true) {};
}