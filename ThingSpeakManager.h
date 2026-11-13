#ifndef THINGSPEAK_MANAGER_H
#define THINGSPEAK_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "Config.h"

class ThingSpeakManager {
private:
    unsigned long lastSendTime;
    int lastResponseCode;
    bool lastSendSuccess;

public:
    ThingSpeakManager();
    void begin();
    
    // Envío periódico automático (llamado desde loop)
    void update(float temp, float hum, float potVoltage);

    // Envío inmediato (usado por /thingup)
    bool sendData(float temp, float hum, float potVoltage);

    // Getters
    bool getLastSendSuccess() const { return lastSendSuccess; }
    int getLastResponseCode() const { return lastResponseCode; }
    unsigned long getLastSendTime() const { return lastSendTime; }
};

#endif // THINGSPEAK_MANAGER_H
