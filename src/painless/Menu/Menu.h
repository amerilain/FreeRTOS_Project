#ifndef RP2040_FREERTOS_IRQ_MENU_H
#define RP2040_FREERTOS_IRQ_MENU_H

#include "painless/tftdisplay.h"
#include "painless/SharedResources.h"
#include <memory>
#include <string>
#include <vector>


// Base Menu class
class Menu {

public:

    Menu(std::shared_ptr<TFTDisplay> disp, std::shared_ptr<SharedResources> resources);
    std::shared_ptr<TFTDisplay> display;
    void event(SharedResources::EventType event);

    bool ssidEntered = false;
    bool passwordEntered = false;

private:
    void mainmenu();
    void statusmenu();
    void Co2Setmenu();
    void wifimenu();
    void credentialsmenu();
    void connectwifimenu();
    // share pointer to share resources
    std::shared_ptr<SharedResources> resources;
    int maxScroll = 0;
    int position = 0;
    int selected = 0;
    uint16_t co2 = 0;
    enum EventType {
        ROT_CLOCKWISE,
        ROT_COUNTER_CLOCKWISE,
        ROT_PRESSED,
        SW0_PRESSED,
        SW1_PRESSED,
        SW2_PRESSED,
        Nothing
    } eventType = Nothing;
    enum MenuEventType {
        mainMenu,
        statusMode,
        Co2SetMode,
        wifiMenu,
        connectwifi,
        enterCredentials,
        credentialsMenu,
        SSID,
        PASSWORD,
        errorStatus
    } MenuEventType = mainMenu;

    SharedResources::EventType events;

    enum key{
        up,
        down
    }key = up;
    enum keyboard{
        capital = 0,
        small = 1,
        number = 2
    } keyboard = capital;
  const uint8_t wifi_signal[10] =
            {
                    0x08, 0x04, 0x12, 0x09, 0x65, 0x65, 0x09, 0x12,
                    0x04, 0x08
            };
    std::array<char, 26> alphabet = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
    std::array<char, 26> lowercase_alphabet = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    std::array<char, 36> special_chars_numbers = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                                  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
                                                  '-', '_', '=', '+', '[', ']', '{', '}', ';', ':',
                                                  '\'', '\"', ',', '.', '<', '>'};
    std::vector<char> currentCharBuff;
    std::vector<char> finalssid;
    std::vector<char> password;
    std:: string displyString;
    char ssid2[25]= " ";
    char password2[25] = " ";

};
#endif // RP2040_FREERTOS_IRQ_MENU_H
