#include <iostream>
#include <sstream>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hardware/gpio.h"
#include "PicoOsUart.h"
#include "ssd1306.h"


#include "hardware/timer.h"

extern "C" {
    uint32_t read_runtime_ctr(void) {
        return timer_hw->timerawl;
    }
}

SemaphoreHandle_t gpio_sem;

void display_task(void *parm);
void rot_and_button_handler_irq(void *parm);

int main() {
    gpio_sem = xSemaphoreCreateBinary();

    //xTaskCreate(display_task, "SSD1306", 512, (void*) nullptr,
                //tskIDLE_PRIORITY + 2, nullptr);

    xTaskCreate(rot_and_button_handler_irq, "Interrupt", 512, (void *) nullptr,
                tskIDLE_PRIORITY + 1, nullptr);

    vTaskStartScheduler();



    while(true){};
}

#include "ssd1306os.h"
#include "painless/tftdisplay.h"
#define I2C1_SDA_PIN 14
#define I2C1_SCL_PIN 15
#define I2C1_BAUD_RATE 400000

void display_task(void *param) {
    auto i2cbus{std::make_shared<PicoI2C>(1, 400000)};
    auto display{std::make_shared<TFTDisplay>( i2cbus)};
    display->fill(0);
    display->text("Painless Aaban", 0, 0);
    display->show();

    while(true) {
        vTaskDelay(10);
    }
}

#include "Button_Rotary_irq/Interrupt_Handler.h"

uint8_t rotA = 10;
uint8_t rotB = 11;
uint8_t rotP = 12;
uint8_t SW0 = 9;
uint8_t  SW1 = 8;
uint8_t  SW2 = 7;



void rot_and_button_handler_irq(void *param) {

    // Handle the rotary encoder and button
    Interrupt_Handler rothandlerA(rotA);

    //Interrupt_Handler rot2(rotB, true);
    Interrupt_Handler rothandlerP(rotP);
    Interrupt_Handler sw0(SW0);
    Interrupt_Handler sw1(SW1);
    Interrupt_Handler sw2(SW2);

    auto i2cbus{std::make_shared<PicoI2C>(1, 400000)};
    auto display{std::make_shared<TFTDisplay>( i2cbus)};


    while(true) {
        if (rothandlerA.count == 1) {
            display->fill(0);
            display->text("Turned Clockwise", 0, 0);
            display->show();
            rothandlerA.count = 0;
        }
        else if (rothandlerA.count == -1) {
            display->fill(0);
            display->text("Turned Counter-Clockwise", 0, 0);
            display->show();
            rothandlerA.count = 0;
        }
        else if (rothandlerP.count == 1) {
            display->fill(0);
            display->text("Rot Pressed", 0, 0);
            display->show();
            rothandlerP.count = 0;
        }
        else if (sw0.count == 1) {
            display->fill(0);
            display->text("Sw0 Pressed", 0, 0);
            display->show();
            sw0.count = 0;
        }
        else if (sw1.count == 1) {
            display->fill(0);
            display->text("Sw1 Pressed", 0, 0);
            display->show();
            sw1.count = 0;
        }
        else if (sw2.count == 1) {
            display->fill(0);
            display->text("Sw2 Pressed", 0, 0);
            display->show();
            sw2.count = 0;
        }
    }
    vTaskDelay(10);
}