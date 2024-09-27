#ifndef EEPROM_H
#define EEPROM_H

#include <memory>
#include "PicoI2C.h"  // Use PicoI2C class for higher-level I2C operations

class EEPROM {
public:
    // Constructor now takes a std::shared_ptr<PicoI2C>
    explicit EEPROM(std::shared_ptr<PicoI2C> i2c, uint16_t device_address);
    ~EEPROM();

    void writeToMemory(uint16_t memory_address, uint8_t data);
    uint8_t readFromMemory(uint16_t memory_address);
    void clearEEPROM();

    void writeToMemory(uint16_t memory_address, const uint8_t *data, size_t length);

    void readFromMemory(uint16_t memory_address, uint8_t *data, size_t length);

private:
    std::shared_ptr<PicoI2C> i2c;  // Use shared_ptr to manage PicoI2C instance
    uint16_t device_address;

    uint16_t crc16(const uint8_t* data_p, size_t length);
    bool writeByte(uint16_t memory_address, uint8_t data);
    bool read_byte(uint16_t memory_address, uint8_t &data);

};

#endif // EEPROM_H