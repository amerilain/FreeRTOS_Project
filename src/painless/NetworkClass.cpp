//
// Created by iamna on 01/10/2024.
//

#include "NetworkClass.h"

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
        bool pass = run_tls_client_test(NULL, 0, TLS_CLIENT_SERVER, TLS_CLIENT_HTTP_REQUEST, TLS_CLIENT_TIMEOUT_SECS);
}




