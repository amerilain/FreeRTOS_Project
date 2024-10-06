//
// Created by iamna on 04/10/2024.
//

#ifndef RP2040_FREERTOS_IRQ_CO2SETMENU_H
#define RP2040_FREERTOS_IRQ_CO2SETMENU_H

#include "Menu.h"
#include "painless/tftdisplay.h"
#include "MainMenu.h"

class Co2SetMenu : public Menu {
/*public:
    Co2SetMenu() : co2Setpoint(1000) {}

    Menu* handleInput(int input) override {
        switch (input) {
            case MainMenu::RotaryA:
                ++co2Setpoint;
                break;
            case MainMenu::RotaryB:
                --co2Setpoint;
                break;
            case MainMenu::RotaryP:
                // Go back to the main menu
                //return new MainMenu(std::shared_ptr<PicoI2C>());
        }
        return nullptr;
    }

    void display() override {
        // Display CO2 setpoint
        //TFTDisplay::displayCo2Setpoint(co2Setpoint); // Assumes TFTDisplay has this method
    }

private:
    int co2Setpoint;*/
};

#endif //RP2040_FREERTOS_IRQ_CO2SETMENU_H
