//
// Created by iamna on 01/10/2024.
//
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

/*extern "C" {
void tls_test(void);
}
void tls_task(void *param)
{
    tls_test();
    while(true) {
        vTaskDelay(100);
    }
}*/

#include "painless/NetworkClass.h"

void tls_task(void *param)
{
    NetworkClass network("Nadim", "nadimahmed");
    bool network_status =false;
    while(true) {
        if (network_status == false)
        {
            network.connect();
            network_status = true;
        }
        if(network_status == true)
        {
            network.recieve();
            vTaskDelay(20000);
        }

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