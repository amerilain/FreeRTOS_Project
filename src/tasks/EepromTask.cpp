#include "eeprom/EEPROM.h"
#include "FreeRTOS.h"
#include "task.h"
/*
extern EEPROM eeprom;
bool settingsUpdated = false;

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
*/
