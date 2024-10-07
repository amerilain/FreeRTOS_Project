#ifndef EEPROM_H
#define EEPROM_H

#include <memory>
#include "PicoI2C.h"

#define BLOCK_SIZE 64  // Standard block size (including size + CRC)

class EEPROM {
public:
    explicit EEPROM(std::shared_ptr<PicoI2C> i2c, uint16_t device_address);
    ~EEPROM();

    // Updated methods to write and read with block size, metadata, and CRC
    void writeToMemory(uint16_t memory_address, const uint8_t* data, size_t length);
    void readFromMemory(uint16_t memory_address, uint8_t* data, size_t length);
private:
    std::shared_ptr<PicoI2C> i2c;
    uint16_t device_address;

<<<<<<< HEAD
    // CRC Calculation
    uint16_t crc16(const uint8_t* data, size_t length);
=======
    uint16_t crc16(const uint8_t* data_p, size_t length);

>>>>>>> eec25e6d4ffb37645dd10088720d09927bbf9738
};

#endif // EEPROM_H