//
// Created by iamna on 29/09/2024.
//

#include "co2valve.h"

co2valve::co2valve(int valve_pin): valve_pin(valve_pin){
    gpio_init(valve_pin);
    gpio_set_dir(valve_pin, GPIO_OUT);
    gpio_pull_up(valve_pin);

}

void co2valve::open() {
    gpio_put(valve_pin, 1);
}

void co2valve::close() {
    gpio_put(valve_pin, 0);
}

void co2valve::setValve(int value) {
   // valve = value;
}

int co2valve::getValve() {
    //return valve;
}

