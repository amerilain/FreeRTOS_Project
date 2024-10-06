//
// Created by iamna on 02/10/2024.
//

#include "SharedResources.h"

SharedResources::SharedResources() {
    co2 = 0;
    tem = 0;
    RH = 0;
    fanSpeed = 0;
    co2SP = 300;
    xbuttonQueue = xQueueCreate(10, sizeof(EventType));
}

int SharedResources::getCo2() {
    return co2;
}

void SharedResources::setCo2(int co2) {
    SharedResources::co2 = co2;
}

int SharedResources::getTem() {
    return tem;
}

void  SharedResources::setTem(int tem) {
    SharedResources::tem = tem;
}

int SharedResources::getRH() {
    return RH;
}

void SharedResources::setRH(int RH) {
    SharedResources::RH = RH;
}

int SharedResources::getFanSpeed() {
    return fanSpeed;
}

void SharedResources::setFanSpeed(int fanSpeed) {
    SharedResources::fanSpeed = fanSpeed;
}

int SharedResources::getCo2SP() {
    return co2SP;
}

void SharedResources::setCo2SP(int co2SP) {
    SharedResources::co2SP = co2SP;
}

int SharedResources::getPressure() {
    return pressure;
}

void SharedResources::setPressure(int pressure) {
    SharedResources::pressure = pressure;
}
