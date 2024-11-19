#ifndef RP2040_FREERTOS_IRQ_NETWORKCLASS_H
#define RP2040_FREERTOS_IRQ_NETWORKCLASS_H

#include <mbedtls/debug.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "queue.h"

#include "task.h"
#include "timers.h"
#include "Sources/SharedResources.h"

#define TLS_CLIENT_SERVER        "api.thingspeak.com"

#define TLS_CLIENT_TIMEOUT_SECS  15


extern "C" {
bool run_tls_client_test(const uint8_t *cert, size_t cert_len, const char *server, const char *request, int timeout);
int get_co2_setpoint();
}

class NetworkClass {
public:
  NetworkClass(std::shared_ptr<SharedResources> sharedResources );
    void init();
    void connect();
    void recieve();
    void send(int co2, int tem, int rh, int fanSpeed, int Co2_SetPoint);
    void sendAndreceive(int co2, int tem, int rh, int fanSpeed, int Co2_SetPoint);
    void setCredentials(char* ssid, char* password);
    int Co2_SetPoint;
    bool transmit = false;

private:
    char ssid[32];
    char password[32];
    std::shared_ptr<SharedResources> resources;
    uint32_t event;

    const char *req = "POST /talkbacks/53261/commands/execute.json HTTP/1.1\r\n"
                      "Host: api.thingspeak.com\r\n"
                      "User-Agent: PicoW\r\n"
                      "Accept: */*\r\n"
                      "Content-Length: 24\r\n"
                      "Content-Type: application/x-www-form-urlencoded\r\n"
                      "\r\n"
                      "api_key=ZZ4SW85BXQ6W18HV";


    const char *req2 = "POST /update.json HTTP/1.1\r\n"
                      "Host: api.thingspeak.com\r\n"
                      "User-Agent: PicoW\r\n"
                      "Accept: */*\r\n"
                      "Content-Length: 65\r\n"
                      "Content-Type: application/x-www-form-urlencoded\r\n"
                      "\r\n"
                      "field1=370&api_key=1WWH2NWXSM53URR5&talkback_key=371DAWENQKI6J8DD";


};


#endif //RP2040_FREERTOS_IRQ_NETWORKCLASS_H
