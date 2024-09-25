#ifndef EEPROM_H
#define EEPROM_H

#include "PicoI2C.h"  // Use PicoI2C class for higher-level I2C operations

class EEPROM {
public:
    // Constructor now takes a PicoI2C pointer, not i2c_inst_t
    explicit EEPROM(PicoI2C* i2c, uint16_t device_address);
    ~EEPROM();

    void writeToMemory(uint16_t memory_address, uint8_t data);
    uint8_t readFromMemory(uint16_t memory_address);
    void clearEEPROM();

private:
    PicoI2C* i2c;  // Now using PicoI2C class instead of i2c_inst_t
    uint16_t device_address;

    uint16_t crc16(const uint8_t* data_p, size_t length);
    bool writeByte(uint16_t memory_address, uint8_t data);
    bool read_byte(uint16_t memory_address, uint8_t &data);
};

#endif // EEPROM_H