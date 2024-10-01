#include <iostream>
#include <sstream>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"  // For communication between tasks
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
QueueHandle_t event_queue;  // Queue for communicating events

void display_task(void *parm);
void rot_and_button_handler_irq(void *parm);

enum EventType {
    ROT_CLOCKWISE,
    ROT_COUNTER_CLOCKWISE,
    ROT_PRESSED,
    SW0_PRESSED,
    SW1_PRESSED,
    SW2_PRESSED
};

struct DisplayEvent {
    EventType event_type;
};

int main() {
    gpio_sem = xSemaphoreCreateBinary();
    event_queue = xQueueCreate(10, sizeof(DisplayEvent));  // Create queue with size 10

    xTaskCreate(display_task, "display", 512, (void*) nullptr, tskIDLE_PRIORITY + 1, nullptr);
    xTaskCreate(rot_and_button_handler_irq, "Interrupt", 512, (void *) nullptr, tskIDLE_PRIORITY + 1, nullptr);

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
    auto display{std::make_shared<TFTDisplay>(i2cbus)};
    display->fill(0);
    display->text("Painless Aaban", 0, 0);
    display->show();

    DisplayEvent event;

    while(true) {
        if (xQueueReceive(event_queue, &event, portMAX_DELAY) == pdPASS) {  // Wait for an event from the queue
            display->fill(0);
            switch (event.event_type) {
                case ROT_CLOCKWISE:
                    display->text("Turned Clockwise", 0, 0);
                    break;
                case ROT_COUNTER_CLOCKWISE:
                    display->text("Turned Counter-Clockwise", 0, 0);
                    break;
                case ROT_PRESSED:
                    display->text("Rot Pressed", 0, 0);
                    break;
                case SW0_PRESSED:
                    display->text("Sw0 Pressed", 0, 0);
                    break;
                case SW1_PRESSED:
                    display->text("Sw1 Pressed", 0, 0);
                    break;
                case SW2_PRESSED:
                    display->text("Sw2 Pressed", 0, 0);
                    break;
            }
            display->show();
        }
    }
}

#include "Button_Rotary_irq/Interrupt_Handler.h"

uint8_t rotA = 10;
uint8_t rotB = 11;
uint8_t rotP = 12;
uint8_t SW0 = 9;
uint8_t SW1 = 8;
uint8_t SW2 = 7;

void rot_and_button_handler_irq(void *param) {
    Interrupt_Handler rothandlerA(rotA);
    Interrupt_Handler rothandlerP(rotP);
    Interrupt_Handler sw0(SW0);
    Interrupt_Handler sw1(SW1);
    Interrupt_Handler sw2(SW2);

    while(true) {
        DisplayEvent event;

        if (rothandlerA.count == 1) {
            event.event_type = ROT_CLOCKWISE;
            xQueueSend(event_queue, &event, portMAX_DELAY);  // Send event to display task
            rothandlerA.count = 0;
        }
        else if (rothandlerA.count == -1) {
            event.event_type = ROT_COUNTER_CLOCKWISE;
            xQueueSend(event_queue, &event, portMAX_DELAY);
            rothandlerA.count = 0;
        }
        else if (rothandlerP.count == 1) {
            event.event_type = ROT_PRESSED;
            xQueueSend(event_queue, &event, portMAX_DELAY);
            rothandlerP.count = 0;
        }
        else if (sw0.count == 1) {
            event.event_type = SW0_PRESSED;
            xQueueSend(event_queue, &event, portMAX_DELAY);
            sw0.count = 0;
        }
        else if (sw1.count == 1) {
            event.event_type = SW1_PRESSED;
            xQueueSend(event_queue, &event, portMAX_DELAY);
            sw1.count = 0;
        }
        else if (sw2.count == 1) {
            event.event_type = SW2_PRESSED;
            xQueueSend(event_queue, &event, portMAX_DELAY);
            sw2.count = 0;
        }

        vTaskDelay(10);
    }
}
