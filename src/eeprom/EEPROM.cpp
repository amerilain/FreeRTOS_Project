#include "EEPROM.h"
#include "PicoI2C.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <cstdio>

#define SDA_PIN 16
#define SCL_PIN 17
#define EEPROM_SIZE 2048
#define I2C_WAIT_TIME 5

SemaphoreHandle_t i2c_mutex;

// Constructor now takes a PicoI2C object and initializes the I2C instance
EEPROM::EEPROM(PicoI2C* i2c, uint16_t device_address)
        : i2c(i2c), device_address(device_address) {
    // PicoI2C class handles the I2C initialization, so no need for low-level gpio or i2c_init here
}

EEPROM::~EEPROM() {
    // No cleanup required for PicoI2C, as it manages itself
}

uint16_t EEPROM::crc16(const uint8_t* data_p, size_t length) {
    uint8_t x;
    uint16_t crc = 0xFFFF;
    while (length--) {
        x = crc >> 8 ^ *data_p++;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
    }
    return crc;
}

void EEPROM::writeToMemory(uint16_t memory_address, uint8_t data) {
    if (memory_address > EEPROM_SIZE - 3) {
        printf("Memory address exceeds EEPROM size.\n");
        return;
    }

    if (xSemaphoreTake(i2c_mutex, portMAX_DELAY) == pdTRUE) {
        uint8_t buffer[1] = {data};
        i2c->write(this->device_address, buffer, 1);  // Use PicoI2C's write method

        vTaskDelay(pdMS_TO_TICKS(I2C_WAIT_TIME));  // FreeRTOS-compliant delay

        uint16_t crc = crc16(&data, 1);
        uint8_t crc_highByte = (crc >> 8) & 0xFF;
        uint8_t crc_lowByte = crc & 0xFF;

        uint8_t crc_buffer[2] = {crc_highByte, crc_lowByte};
        i2c->write(this->device_address, crc_buffer, 2);  // Write CRC using PicoI2C's write method

        vTaskDelay(pdMS_TO_TICKS(I2C_WAIT_TIME));  // Delay for I2C transaction

        xSemaphoreGive(i2c_mutex);  // Release mutex
    } else {
        printf("Failed to take mutex for EEPROM write operation.\n");
    }
}

uint8_t EEPROM::readFromMemory(uint16_t memory_address) {
    uint8_t data = 0;

    if (xSemaphoreTake(i2c_mutex, portMAX_DELAY) == pdTRUE) {
        uint8_t buffer[1];
        i2c->read(this->device_address, buffer, 1);  // Use PicoI2C's read method

        data = buffer[0];  // Store the read data

        xSemaphoreGive(i2c_mutex);  // Release mutex
    } else {
        printf("Failed to take mutex for EEPROM read operation.\n");
    }

    return data;  // Return the read data
}

void EEPROM::clearEEPROM() {
    uint8_t fill = 0xFF;
    for (uint16_t i = 0; i < EEPROM_SIZE - 2; i += 3) {
        this->writeToMemory(i, fill);
    }
}