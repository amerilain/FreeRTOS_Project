#include "EEPROM.h"
#include "PicoI2C.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <cstdio>
#include <cstring>

#define SDA_PIN 16
#define SCL_PIN 17
#define EEPROM_SIZE 2048
#define I2C_WAIT_TIME 5

extern SemaphoreHandle_t i2c_mutex;

// Constructor now takes a PicoI2C object and initializes the I2C instance
EEPROM::EEPROM(std::shared_ptr<PicoI2C> i2c, uint16_t device_address)
        : i2c(i2c), device_address(device_address) {
    // PicoI2C class handles the I2C initialization
}

EEPROM::~EEPROM() {
    // No cleanup required for PicoI2C, as it manages itself
}

// Write multiple bytes (e.g., a string) to EEPROM
void EEPROM::writeToMemory(uint16_t memory_address, const uint8_t* data, size_t length) {
    if (memory_address + length > EEPROM_SIZE) {
        printf("Memory address exceeds EEPROM size.\n");
        return;
    }

    if (xSemaphoreTake(i2c_mutex, portMAX_DELAY) == pdTRUE) {
        uint8_t* buffer = new uint8_t[length + 2];
        buffer[0] = (memory_address >> 8) & 0xFF; // High byte of memory address
        buffer[1] = memory_address & 0xFF;        // Low byte of memory address
        memcpy(buffer + 2, data, length);         // Copy data after address

        i2c->write(this->device_address, buffer, length + 2);  // Write memory address + data
        vTaskDelay(pdMS_TO_TICKS(I2C_WAIT_TIME));              // FreeRTOS-compliant delay

        delete[] buffer;  // Free the allocated buffer
        xSemaphoreGive(i2c_mutex);  // Release mutex
    } else {
        printf("Failed to take mutex for EEPROM write operation.\n");
    }
}

void EEPROM::readFromMemory(uint16_t memory_address, uint8_t* data, size_t length) {
    if (xSemaphoreTake(i2c_mutex, portMAX_DELAY) == pdTRUE) {
        uint8_t address_buffer[2];
        address_buffer[0] = (memory_address >> 8) & 0xFF;  // High byte of memory address
        address_buffer[1] = memory_address & 0xFF;         // Low byte of memory address

        // First, send the memory address to the EEPROM
        i2c->write(this->device_address, address_buffer, 2);

        // Now, read the data starting at that address
        i2c->read(this->device_address, data, length);

        xSemaphoreGive(i2c_mutex);  // Release mutex
    } else {
        printf("Failed to take mutex for EEPROM read operation.\n");
    }
}

