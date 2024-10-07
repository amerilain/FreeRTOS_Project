#include <iostream>
#include <sstream>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hardware/gpio.h"
#include "eeprom/EEPROM.h"
#include "tasks/EepromTask.cpp"
#include "PicoI2C.h"
#include "pico/stdlib.h"
#include <memory>  // Include shared_ptr
#include <cstring>

// Send commands to things speak:
// curl -v -d "command_string=my%20fancy%20command&api_key=ZZ4SW85BXQ6W18HV" http://api.thingspeak.com/talkbacks/53261/commands

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


void Eeprom_Task(void *param) {
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
        vTaskDelay(100);
    }

}

int main() {
    stdio_init_all();

    // Create the I2C task
    xTaskCreate(Eeprom_Task, "Eeprom_Task", 256, NULL, 1, NULL);

    // Start FreeRTOS scheduler
    vTaskStartScheduler();

    while (true) {};
}