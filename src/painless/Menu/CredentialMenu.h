//
// Created by iamna on 04/10/2024.
//

#ifndef RP2040_FREERTOS_IRQ_CREDENTIALMENU_H
#define RP2040_FREERTOS_IRQ_CREDENTIALMENU_H

#include "Menu.h"
#include "MainMenu.h"
#include "painless/tftdisplay.h"

class CredentialMenu : public Menu {
/*private:
    std::string ssid;
    std::string password;
    bool isSsidPhase;
    bool isUpperCase;

public:
    CredentialMenu() : isSsidPhase(true), isUpperCase(false) {}

    Menu* handleInput(int input) override {
        if (isSsidPhase) {
            handleSsidInput(input);
        } else {
            handlePasswordInput(input);
        }
        return nullptr;
    }

    void display() override {
        if (isSsidPhase) {
            TFTDisplay::displaySsidEntry(ssid);  // Assumes TFTDisplay has this method
        } else {
            TFTDisplay::displayPasswordEntry(password);  // Assumes TFTDisplay has this method
        }
    }

private:
    void handleSsidInput(int input) {
        switch (input) {
            case MainMenu::Button0:
                saveSsid();
                break;
            case MainMenu::Button1:
                eraseLastChar(ssid);
                break;
            case MainMenu::Button2:
                toggleCase();
                break;
            default:
                appendChar(ssid, input);
        }
    }

    void handlePasswordInput(int input) {
        switch (input) {
            case MainMenu::Button0:
                savePassword();
                break;
            case MainMenu::Button1:
                eraseLastChar(password);
                break;
            case MainMenu::Button2:
                toggleCase();
                break;
            default:
                appendChar(password, input);
        }
    }

    void saveSsid() {
        isSsidPhase = false; // Move to password phase
    }

    void savePassword() {
        // Implement Wi-Fi connection logic here
    }

    void eraseLastChar(std::string &str) {
        if (!str.empty()) {
            str.pop_back();
        }
    }

    void toggleCase() {
        isUpperCase = !isUpperCase;
    }

    void appendChar(std::string &str, int input) {
        char ch = static_cast<char>(input); // Map input to character
        if (isUpperCase) {
            ch = toupper(ch);
        }
        str += ch;
    }*/
};

#endif //RP2040_FREERTOS_IRQ_CREDENTIALMENU_H
