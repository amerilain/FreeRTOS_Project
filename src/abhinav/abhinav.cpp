//
// Created by abhin on 27/09/2024.
//

#include <iostream>
#include <sstream>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hardware/gpio.h"
#include "PicoOsUart.h"
#include "ssd1306.h"
#include "pico/stdlib.h"


#include "hardware/timer.h"
extern "C" {
uint32_t read_runtime_ctr(void) {
    return timer_hw->timerawl;
}
}

extern "C" {
void tls_test(void);
}
void tls_task(void *param)
{
    tls_test();
    while(true) {
        vTaskDelay(100);
    }
}

int main()
{
    stdio_init_all();
    xTaskCreate(tls_task, "tls test", 6000, (void *) nullptr,
                tskIDLE_PRIORITY + 1, nullptr);

    vTaskStartScheduler();

    while(true){};
}
