//
// Created by iamna on 01/10/2024.
//

#include <string>
#include "NetworkClass.h"
const char *req = "POST /talkbacks/53261/commands/execute.json HTTP/1.1\r\n"
                  "Host: api.thingspeak.com\r\n"
                  "User-Agent: PicoW\r\n"
                  "Accept: */*\r\n"
                  "Content-Length: 24\r\n"
                  "Content-Type: application/x-www-form-urlencoded\r\n"
                  "\r\n"
                  "api_key=ZZ4SW85BXQ6W18HV";

NetworkClass::NetworkClass(const char *ssid, const char *password) {
    strcpy(this->ssid, ssid);
    strcpy(this->password, password);
    init();
}

void NetworkClass::init() {
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return;
    }
    cyw43_arch_enable_sta_mode();

}

void NetworkClass::connect() {
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect\n");
        return;
    }
}

void NetworkClass::recieve() {
        run_tls_client_test(NULL, 0, TLS_CLIENT_SERVER, req, TLS_CLIENT_TIMEOUT_SECS);

    std::string json_data = get_buffer(); // Assuming 'buffer' contains the JSON data

    std::string key = "\"command_string\":\"";
    size_t start = json_data.find(key);
    if (start != std::string::npos) {
        start += key.length();
        size_t end = json_data.find("\"", start);
        if (end != std::string::npos) {
            std::string command_string = json_data.substr(start, end - start);
           printf("command_string: %s\n", command_string.c_str());
        }
    }
        //printf("buffer received at NetworkClass: %s\n", get_buffer());
}




