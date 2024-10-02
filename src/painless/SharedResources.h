//
// Created by iamna on 02/10/2024.
//

#ifndef RP2040_FREERTOS_IRQ_SHAREDRESOURCES_H
#define RP2040_FREERTOS_IRQ_SHAREDRESOURCES_H


class SharedResources {
public:
    SharedResources();
    int getCo2();
    void setCo2(int co2);

    int getTem();
    void setTem(int tem);

    int getRH();
    void setRH(int RH);

    int getFanSpeed();
    void setFanSpeed(int fanSpeed);

    int getCo2SP();
    void setCo2SP(int co2SP);

    int getPressure();
    void setPressure(int pressure);





private:
    int co2;
    int tem;
    int RH;
    int pressure;
    int fanSpeed;
    int co2SP;
};


#endif //RP2040_FREERTOS_IRQ_SHAREDRESOURCES_H
