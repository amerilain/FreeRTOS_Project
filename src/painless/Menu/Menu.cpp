//
// Created by iamna on 04/10/2024.
//

#include "Menu.h"

Menu::Menu(std::shared_ptr<TFTDisplay> disp, std::shared_ptr<SharedResources> resources)
        : display(disp), resources(resources) {
    display->mainMenu();
}

void Menu::mainmenu() {
    if (resources->isRotaryClockwise ) {
        position++;
        resources->isRotaryClockwise = false;
    }
    if (resources->isRotaryCounterClockwise) {
        position--;
        resources->isRotaryCounterClockwise = false;
    }
    if(position > 3) {
        position = 0;
    }
    if(position < 0) {
        position = 3;
    }
    display->selectmenu(position);
    if (resources->isRotaryPressed) {
        resources->isRotaryPressed = false;
        display->fill(0);
        selected = position;
        switch (selected) {
            case 0:
                MenuEventType = Co2SetMode;
                break;
            case 1:
                MenuEventType = statusMode;
                break;
            case 2:
                MenuEventType = wifiMenu;
                display->wifiMMenu();
                break;
            case 3:
                MenuEventType = credentialsMenu;
                break;
        }
    }
}
void Menu::statusmenu() {
    display->displayStatus(resources->getCo2(),resources->getTem(),
                           resources->getRH(),resources->getFanSpeed(),
                           resources->getPressure(), wifi_signal);
    if (resources->isRotaryPressed) {
        resources->isRotaryPressed = false;
        printf("Pressed\n");
        MenuEventType = mainMenu;
        display->mainMenu();
    }
}

void Menu::Co2Setmenu() {
    display->Cc2Setpoint(co2);
    if(resources->isRotaryClockwise){
        resources->isRotaryClockwise = false;
        co2 < 1500 ? co2+=100 : 1500;
        printf("Co2 increase: %d\n", co2);
    }
    if(resources->isRotaryCounterClockwise){
        resources->isRotaryCounterClockwise = false;
        co2 > 200 ? co2-=100 : 200;
        printf("Co2 decrese: %d\n", co2);
    }
    if(resources->isRotaryPressed){
        resources->isRotaryPressed = false;
        resources->setCo2SP(co2);
        printf("set Co2 SP: %d\n", co2);
        MenuEventType = mainMenu;
        display->fill(0);
        display->mainMenu();
    }
}

void Menu::wifimenu() {
    if (resources->isRotaryClockwise) {
        position++;
        resources->isRotaryClockwise = false;
    }
    if (resources->isRotaryCounterClockwise) {
        position--;
        resources->isRotaryCounterClockwise = false;
    }
    if(position > 2) {
        position = 0;
    }
    if(position < 0) {
        position = 2;
    }
    display->selectwifimenu(position);
    if (resources->isRotaryPressed) {
        resources->isRotaryPressed = false;
        display->fill(0);
        selected = position;
        switch (selected) {
            case 0:
                MenuEventType = connectwifi;
                break;
            case 1:
                MenuEventType = credentialsMenu;
                display->credenttialsMenu();
                break;
            case 2:
                MenuEventType = mainMenu;
                display->mainMenu();
                break;
        }
    }
}
void Menu::credentialsmenu() {
    if (resources->isRotaryClockwise) {
        position++;
        resources->isRotaryClockwise = false;
    }
    if (resources->isRotaryCounterClockwise) {
        position--;
        resources->isRotaryCounterClockwise = false;
    }
    if(position > 2) {
        position = 0;
    }
    if(position < 0) {
        position = 2;
    }
    display->selectwifimenu(position);
    if(resources->isRotaryPressed){
        resources->isRotaryPressed = false;
        display->fill(0);
        selected = position;
        switch (selected) {
            case 0:
                MenuEventType = SSID;
                break;
            case 1:
                MenuEventType = PASSWORD;
                break;
            case 2:
                MenuEventType = wifiMenu;
                display->wifiMMenu();
                break;
        }
    }
}
void Menu::event(SharedResources::EventType event) {
    switch (MenuEventType ) {
        case mainMenu:
            mainmenu();
            break;
        case statusMode:
            statusmenu();
            break;
        case Co2SetMode:
            Co2Setmenu();
            break;
        case wifiMenu:
            wifimenu();
            break;
        case credentialsMenu:
           credentialsmenu();
            break;
        case SSID:
            display->fill(0);
            display->text("Enter SSID", 0, 20);
            display->show();
            vTaskDelay(pdMS_TO_TICKS(4000));
            MenuEventType = enterCredentials;
            keyboard= capital;
            display->displayKeyboard(keyboard);
            display->selectChar(0);
            maxScroll = alphabet.size()-1;
            position = 0;

            break;
        case PASSWORD:
            display->fill(0);
            display->text("Enter Password", 0, 20);
            display->show();
            vTaskDelay(pdMS_TO_TICKS(4000));
            MenuEventType = enterCredentials;
            keyboard= capital;
            display->displayKeyboard(keyboard);
            display->selectChar(0);
            maxScroll = alphabet.size()-1;
            position = 0;

            break;
        case connectwifi:
            MenuEventType = mainMenu;
            resources->setSSID(ssid2);
            resources->setPassword(password2);
            printf("SSID: %s\n", ssid2);
            printf("Password: %s\n", password2);
            resources->credentialsEntered = true;
            if(resources->credentialsEntered){
                display->fill(0);
                display->text("Connecting...", 0, 20);
                display->show();
                vTaskDelay(pdMS_TO_TICKS(4000));
            }
            display->mainMenu();
            display->selectmenu(0);
            break;

        case enterCredentials:
            // Change the keyboard to the next one Capital -> Small -> Number
            if (resources->isSW2Pressed) {
                resources->isSW2Pressed = false;
                if (keyboard == capital) {
                    maxScroll = lowercase_alphabet.size() - 1;                //lowercase_alphabet.size() - 1;
                    keyboard = small;
                } else if (keyboard == small) {
                    maxScroll = special_chars_numbers.size() - 1;               //special_chars_numbers.size() - 1;
                    keyboard = number;
                } else {
                    keyboard = capital;
                    maxScroll = alphabet.size() - 1;               //alphabet.size() - 1;
                }
                display->displayKeyboard(keyboard);
            }

            // Select the character
            if(resources->isRotaryClockwise){
                resources->isRotaryClockwise = false;
                position++;
            }
            if(resources->isRotaryCounterClockwise){
                resources->isRotaryCounterClockwise = false;
                position--;
            }
            if(position > maxScroll) {
                position = 0;
            }
            if(position < 0) {
                position = maxScroll;
            }
            display->selectChar(position);
            // Add the selected character to the buffer
            if (resources->isRotaryPressed){
                resources->isRotaryPressed = false;
                if(keyboard == capital){
                   currentCharBuff.push_back(alphabet[position]);
                }
                else if(keyboard == small) {
                    currentCharBuff.push_back(lowercase_alphabet[position]);
                }
                else {
                    currentCharBuff.push_back(special_chars_numbers[position]);
                }
                displyString = std::string(currentCharBuff.begin(), currentCharBuff.end());
                display->displayString(displyString);
                display->selectChar(position);
            }

            // Erase the last character
            if(resources->isSW1Pressed){
                resources->isSW1Pressed = false;
                if(!currentCharBuff.empty()){
                   currentCharBuff.pop_back();
                    displyString = std::string(currentCharBuff.begin(), currentCharBuff.end());
                    display->displayString(displyString);
                }
            }

            if (resources->isSW0Pressed) { // Save the entered string
                resources->isSW0Pressed = false;
                if (!displyString.empty()) { // If the Entered string is not empty
                    displyString = std::string(currentCharBuff.begin(), currentCharBuff.end());
                   // std::cout << displyString << std::endl;
                    printf("Entered: %s\n", displyString.c_str());
                    if (!ssidEntered) { // If the ssid is not entered save it to ssid2
                        std::copy(currentCharBuff.begin(), currentCharBuff.end(), ssid2);
                        ssid2[displyString.size()] = '\0';
                        currentCharBuff.clear();
                        ssidEntered = true;
                        MenuEventType = credentialsMenu;
                        display->credenttialsMenu();
                        display->selctcredenttials(0);
                    } else if (!passwordEntered) { // If the password is not entered save it to password2
                        std::copy(currentCharBuff.begin(), currentCharBuff.end(), password2);
                        password2[displyString.size()] = '\0';
                        currentCharBuff.clear();
                        passwordEntered = true;
                        MenuEventType = credentialsMenu;
                        display->credenttialsMenu();
                        display->selctcredenttials(0);
                    }

                }
            }
            break;

        case errorStatus:
            break;

    }
}

