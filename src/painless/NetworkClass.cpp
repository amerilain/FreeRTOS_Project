//
// Created by iamna on 01/10/2024.
//

#include <string>
#include "NetworkClass.h"



NetworkClass::NetworkClass() {
    Co2_SetPoint = 0;
}

void NetworkClass::init() {
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return;
    }
    cyw43_arch_enable_sta_mode();

}

void NetworkClass::setCredentials(char* ssid, char* password) {
    strcpy(this->ssid, ssid);
    strcpy(this->password, password);
}

void NetworkClass::connect() {
    init();
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect\n");
        return;
    }
}

void NetworkClass::recieve() {
    run_tls_client_test(NULL, 0, TLS_CLIENT_SERVER, req, TLS_CLIENT_TIMEOUT_SECS);

    Co2_SetPoint = get_co2_setpoint();

/*    *//*std::string json_data = get_buffer(); // Assuming 'buffer' contains the JSON data

    std::string key = "\"command_string\":\"";
    size_t start = json_data.find(key);
    if (start != std::string::npos) {
        start += key.length();
        size_t end = json_data.find("\"", start);
        if (end != std::string::npos) {
            std::string command_string = json_data.substr(start, end - start);
            Co2_SetPoint = std::stoi(command_string);
           printf("command_string: %s\n", command_string.c_str());
            printf("Co2_SetPoint: %d\n", Co2_SetPoint);
        }*//*
    }*/
        //printf("buffer received at NetworkClass: %s\n", get_buffer());

   printf("Co2_SetPoint: %d\n",Co2_SetPoint);
}

void NetworkClass::send( int co2, int temp, int hum, int fan, int AP) {
    char resp[256];
    sprintf(resp, "POST /update?api_key=90GTB0DYRPZN9G6Y&field1=%d&field2=%d&field3=%d&field4=%d&field5=%d HTTP/1.1\r\n"
                 "Host: api.thingspeak.com\r\n"
                 "\r\n", co2, temp, hum, fan, AP);
    run_tls_client_test(NULL, 0, TLS_CLIENT_SERVER, resp, TLS_CLIENT_TIMEOUT_SECS);
}




