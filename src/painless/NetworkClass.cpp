//
// Created by iamna on 01/10/2024.
//

#include <string>
#include "NetworkClass.h"



NetworkClass::NetworkClass(std::shared_ptr<SharedResources> sharedResources) : resources(sharedResources) {
   Co2_SetPoint = 0;
 /*   //dataSendTimer = xTimerCreate("dataSendTimer", pdMS_TO_TICKS(1000), pdTRUE, this, dataSendTimerCallback);
   dataReceiveTimer = xTimerCreate("dataReceiveTimer", pdMS_TO_TICKS(10000), pdTRUE, this, dataReceiveTimerCallback);
   // xTimerStart(dataSendTimer, 0);
    xTimerStart(dataReceiveTimer, portMAX_DELAY);*/
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
   printf("Co2_SetPoint: %d\n",Co2_SetPoint);
}

void NetworkClass::send( int co2, int temp, int hum, int fan, int AP) {
    char resp[256];
    sprintf(resp, "POST /update?api_key=90GTB0DYRPZN9G6Y&field1=%d&field2=%d&field3=%d&field4=%d&field5=%d HTTP/1.1\r\n"
                 "Host: api.thingspeak.com\r\n"
                 "\r\n", co2, temp, hum, fan, AP);
    run_tls_client_test(NULL, 0, TLS_CLIENT_SERVER, resp, TLS_CLIENT_TIMEOUT_SECS);
}

void NetworkClass::dataSendTimerCallback(TimerHandle_t xTimer) {
    auto instance = static_cast<NetworkClass *>(pvTimerGetTimerID(xTimer));
    if(instance->transmit) {
        int co2 = instance->resources->getCo2();
        int temp = instance->resources->getTem();
        int hum = instance->resources->getRH();
        int fan = instance->resources->getFanSpeed();
        int AP = instance->resources->getPressure();
        instance->send(co2, temp, hum, fan, AP);
        printf("Data sent\n");
    }

}

void NetworkClass::dataReceiveTimerCallback(TimerHandle_t xTimer) {
    auto instance = static_cast<NetworkClass *>(pvTimerGetTimerID(xTimer));
    //vTaskDelay(pdMS_TO_TICKS(20000));
    if(instance->transmit){
        printf("waiting to received\n");
        instance->recieve();
        printf("Data received\n");
    }

}




