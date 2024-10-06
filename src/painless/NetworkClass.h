//
// Created by iamna on 01/10/2024.
//

#ifndef RP2040_FREERTOS_IRQ_NETWORKCLASS_H
#define RP2040_FREERTOS_IRQ_NETWORKCLASS_H

#include <mbedtls/debug.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "queue.h"


#define TLS_CLIENT_SERVER        "api.thingspeak.com"
/*
#define TLS_CLIENT_HTTP_REQUEST  "GET /talkbacks/53261/commands/execute.json?api_key=ZZ4SW85BXQ6W18HV HTTP/1.1\r\n" \
                                 "Host: " TLS_CLIENT_SERVER "\r\n" \
                                 "Connection: close\r\n" \
                                 "\r\n"
*/
//extern const char *req;


#define TLS_CLIENT_TIMEOUT_SECS  15


extern "C" {
bool run_tls_client_test(const uint8_t *cert, size_t cert_len, const char *server, const char *request, int timeout);
int get_co2_setpoint();
}

class NetworkClass {
public:
  NetworkClass();
    void init();
    void connect();
    void recieve();
    void send(int co2, int tem, int rh, int fanSpeed, int pressure);
    void setCredentials(char* ssid, char* password);
    int Co2_SetPoint;
private:
    char ssid[32];
    char password[32];

    uint32_t event;

    const char *req = "POST /talkbacks/53261/commands/execute.json HTTP/1.1\r\n"
                      "Host: api.thingspeak.com\r\n"
                      "User-Agent: PicoW\r\n"
                      "Accept: */*\r\n"
                      "Content-Length: 24\r\n"
                      "Content-Type: application/x-www-form-urlencoded\r\n"
                      "\r\n"
                      "api_key=ZZ4SW85BXQ6W18HV";



};


#endif //RP2040_FREERTOS_IRQ_NETWORKCLASS_H
