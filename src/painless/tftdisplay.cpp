//
// Created by iamna on 09/03/2024.
//

#include <iostream>
#include "tftdisplay.h"
#include "font_petme128_8x8.h"

TFTDisplay::TFTDisplay( std::shared_ptr<PicoI2C> i2c): ssd1306os(i2c) {
    width = 0;
}
void TFTDisplay::displayText(const char *string, const uint8_t *font) {
    fill(0);
    text(string, 0, 0);
    mono_vlsb rb(font, 13, 11);
    blit(rb, 114, 0);
    show();
}

void TFTDisplay::displayMenu(const char *textA, const char *textB) {
    fill(0);
    text(textA, 0, 30);
    text(textB, 0, 40);
    show();
}

void TFTDisplay::displayControlPressureLevel(uint16_t pressure) {
    std::string APmessage = std::to_string(pressure) + " Pa";
    width = (pressure * 128) /120;
    fill(0);
    text("Air Pressure: ", 0, 0);
    text(APmessage, 45, 20);
    rect(0, 40,width , 20, 1, true);
    show();
}

void TFTDisplay::displayControlSpeed(uint16_t speed) {
    std::string FSmessage = std::to_string(speed) + " %";
    width = (speed * 128) / 100;
    fill(0);
    text("Fan Speed: ", 0, 0);
    text(FSmessage, 45, 20);
    rect(0, 40,width , 20, 1, true);
    show();
}
void TFTDisplay::Cc2Setpoint(uint16_t speed) {
    std::string FSmessage = std::to_string(speed) + " ppm";
    width = (speed * 128) / 1500;
    fill(0);
    text("Co2 Set Point ", 0, 0);
    text(FSmessage, 45, 20);
    rect(0, 40,width , 20, 1, true);
    show();
}

void TFTDisplay::welcomeScreen() {
    fill(0);
    text("Welcome to Group", 0, 22);
    text("3's Ventilation", 0, 32);
    text("Project", 38, 42);
    show();
}

void TFTDisplay::mainMenu() {
    fill(0);
    text("Main Menu", 30, 2);
    text("Co2 SetPoint", 0, 20);
    text("Status", 0, 30);
    text( "WiFi Connection", 0, 42);
    text("Status", 0, 54);
    show();
}

void TFTDisplay::wifiMMenu() {
    fill(0);
    text("Connect", 0, 15);
    text("New Credentials", 0, 30);
    text("Return", 0,45 );
    show();
}

void TFTDisplay::selectwifimenu(int selection) {
    if(selection == 0){
        rect(0, 11, 128, 15, 1);
        rect(0, 26, 128, 15, 0);
        rect(0, 41, 128, 15, 0);
    } else if(selection == 1){
        rect(0, 11, 128, 15, 0);
        rect(0, 26, 128, 15, 1);
        rect(0, 41, 128, 15, 0);
    } else if(selection == 2){
        rect(0, 11, 128, 15, 0);
        rect(0, 26, 128, 15, 0);
        rect(0, 41, 128, 15, 1);
    }
    show();
}
void TFTDisplay::selectmenu(int selection) {
    if(selection == 0){
        rect(0, 18, 128, 12, 1);
        rect(0, 28, 128, 12, 0);
        rect(0, 40, 128, 12, 0);
        rect(0, 52, 128, 12, 0);
    } else if(selection == 1){
        rect(0, 18, 128, 12, 0);
        rect(0, 28, 128, 12, 1);
        rect(0, 40, 128, 12, 0);
        rect(0, 52, 128, 12, 0);
    } else if(selection == 2){
        rect(0, 18, 128, 12, 0);
        rect(0, 28, 128, 12, 0);
        rect(0, 40, 128, 12, 1);
        rect(0, 52, 128, 12, 0);
    } else if(selection == 3){
        rect(0, 18, 128, 12, 0);
        rect(0, 28, 128, 12, 0);
        rect(0, 40, 128, 12, 0);
        rect(0, 52, 128, 12, 1);
    }
    show();
}
void TFTDisplay::displayMQTTandWifiMenu(int number) {
    fill(0);
    if (number == 0) {
        text("MQTT", 0, 0);
        text("WiFi", 0, 12);
    } else {
        text("MQTT", 0, 12);
        text("WiFi", 0, 0);
    }
    show();
}
void TFTDisplay::selectChar(int selection) {

    int positionY = 0;

    if (selection > 9 && selection <= 19) {
        selection = selection-10;
        positionY += 12;
    }
    else if (selection > 19 ) {
        selection = selection-20;
        positionY += 24;
    }

    int positionX = selection * 12;

    rect(2 + (positionX), positionY, 12, 12, 1);
    show();
    rect(2 + (positionX), positionY, 12, 12, 0);


}

void TFTDisplay::displaychar(char c, int positionX, int positionY) {
    std::string s(1, c);
    rect( positionX, positionY, 10, 10, 0, true);
    text(s.c_str(), positionX, positionY);

    show();
}
void TFTDisplay::displayStatus(const int co2, const int temp, const int hum, const int fan, const int AP, const uint8_t *wifi_icon) {
    mono_vlsb wifi_sig(wifi_icon, 10, 7);
    std::string fs = "FS  : " + std::to_string(fan) + " %";
    std::string ap = "AP  : " + std::to_string(AP) + " Pa";
    std::string Co2 = "Co2 : " + std::to_string(co2) + " ppm";
    std::string tmp = "Temp: " + std::to_string(temp) + " C";
    std::string rh = "RH  : " + std::to_string(hum) + " %";

    fill(0);
    text("Status", 0, 0);
    line(0, 10, 128, 10, 1);
    text(Co2, 0, 13);
    text(rh, 0, 24);
    text(tmp, 0, 35);
    text(fs, 0, 46);
    text(ap, 0, 57);
    blit(wifi_sig, 114, 0);
    show();
}
void TFTDisplay::displayString(const std::string& string) {
    rect(3, 52, 128, 10, 0, true);
    text(string.c_str(), 4, 52);
    show();
}

void TFTDisplay::displayString(const char* string) {
    rect(3, 52, 128, 10, 0, true);
    text(string, 4, 52);
    show();
}

void TFTDisplay::displayKeyboard(int number) {
    int positionX = 4;
    int positionY = 2;
    int positionY_ref = 0;
    int start = 0;
    int end = 0;
    const char* charArray = nullptr;
    int arraySize = 0;

    switch (number) {
        case 0:
            charArray = alphabet.data();
            arraySize = alphabet.size();
            break;
        case 1:
            charArray = lowercase_alphabet.data();
            arraySize = lowercase_alphabet.size();
            break;
        case 2:
            charArray = special_chars_numbers.data();
            arraySize = special_chars_numbers.size();
            break;
        default:
            return;
    }
    rect(0, 0, 128, 50, 0, true);
    // Iterate through the selected array and display characters
    for (int i = 0; i < arraySize; ++i) {
        std::string c(1, charArray[i]);
        text(c.c_str(), positionX, positionY);
        positionX += 12;
        positionY_ref++;
        if (positionY_ref == 10) {
            positionY += 12;
            positionX = 4;
            positionY_ref = 0;
        }
    }

    // Draw lines
    int lineX = 4;
    for (int i = 0; i < 7; i++) {
        line(lineX, 64, lineX + 12, 64, 1);
        lineX += 18;
    }

    show();
}
void TFTDisplay::credenttialsMenu() {
    fill(0);
    text("SSID", 0, 15);
    text("Password", 0, 30);
    text("Return", 0,45 );
    show();
}
void TFTDisplay::selctcredenttials(int selection) {
    if(selection == 0){
        rect(0, 11, 128, 15, 1);
        rect(0, 26, 128, 15, 0);
        rect(0, 41, 128, 15, 0);
    } else if(selection == 1){
        rect(0, 11, 128, 15, 0);
        rect(0, 26, 128, 15, 1);
        rect(0, 41, 128, 15, 0);
    } else if(selection == 2){
        rect(0, 11, 128, 15, 0);
        rect(0, 26, 128, 15, 0);
        rect(0, 41, 128, 15, 1);
    }
    show();
}
void TFTDisplay::errorScreen() {
    fill(0);
    text("Error", 44, 5);
    text("Pressure Low", 0, 22);
    text("Please restart", 0, 32);
    text("The device", 0, 42);
    show();
}

void TFTDisplay::connecting() {
    fill(0);
    text("Connecting . . .", 0, 30);
    show();
}
void TFTDisplay::notconnected() {
    fill(0);
    text("Not connected", 0, 30);
    show();
}