#include "eeprom/EEPROM.h"
#include "FreeRTOS.h"
#include "task.h"

extern EEPROM eeprom;
bool settingsUpdated = false;

void EepromTask(void *params) {
    while (true) {
        if (settingsUpdated) {
            uint8_t co2_setpoint = ...; // Get updated value
            eeprom.writeToMemory(0x00, co2_setpoint);
            settingsUpdated = false;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

