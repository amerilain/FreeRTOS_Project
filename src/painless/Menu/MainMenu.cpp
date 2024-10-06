//
// Created by iamna on 04/10/2024.
//

#include "MainMenu.h"
#include "StatusMenu.h"
#include "MenuHandler.h"

/*
void MainMenu::displayWindow() {
    // Display the window
    display->selectmenu(position);
    printf("Main Menu displayed %d\n", position);
}

bool MainMenu::event(Menu::MenuEventType e) {
    display->mainMenu();
    switch (e) {
        case RotaryA:
            position++;
            return true;
        case RotaryB:
            position--;
            return true;
        case RotaryButton:
            // Handle the event
             switchMenu(std::make_unique<StatusMenu>(display));
            return true;
    }
    if (position > 2) position = 0;
    if (position < 0) position = 2;
}*/

/*
MainMenu::MainMenu(std::shared_ptr<TFTDisplay> display, MenuHandler& menuHandler): Menu(display), menuHandler(menuHandler) {};

bool MainMenu::event(Menu::MenuEventType e) {
    display->mainMenu();
        switch (e) {
        case RotaryButton:
        menuHandler.switchMenu(std::make_unique<StatusMenu>(display));
        break;
        }

}*/
