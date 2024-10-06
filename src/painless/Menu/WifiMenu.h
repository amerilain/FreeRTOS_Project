//
// Created by iamna on 04/10/2024.
//

#ifndef RP2040_FREERTOS_IRQ_WIFIMENU_H
#define RP2040_FREERTOS_IRQ_WIFIMENU_H

#include "Menu.h"
#include "painless/tftdisplay.h"
#include "MainMenu.h"
#include "CredentialMenu.h"

class WifiMenu : public Menu {
/*public:
    Menu* handleInput(int input) override {
        if (input == MainMenu::RotaryP) {
            return new CredentialMenu();
        }
        return nullptr;
    }

    void display() override {
        // Display Wi-Fi connection options
        //TFTDisplay::displayWifiMenu(); // Assumes TFTDisplay has this method
    }*/
};

// Credential Entry Menu




#endif //RP2040_FREERTOS_IRQ_WIFIMENU_H
