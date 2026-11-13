#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <DHT.h>
#include "Config.h"

class SensorManager {
private:
    DHT dht;
    float currentTemperature;
    float currentHumidity;
    float currentPotVoltage;
    int currentPotRaw;

    unsigned long lastReadTime;
    bool criticalAlertPending;
    bool criticalAlertActive;

    void readSensors();

public:
    SensorManager();
    void begin();
    void update();

    // Lectura inmediata manual (usado por comandos como /thingup)
    void forceReadPot();

    // Getters
    float getTemperature() const { return currentTemperature; }
    float getHumidity() const { return currentHumidity; }
    float getPotVoltage() const { return currentPotVoltage; }
    int getPotRaw() const { return currentPotRaw; }

    // Detección de umbral crítico y alerta
    bool isPotVoltageCritical() const;
    bool takeCriticalAlertEvent(); // Devuelve true una sola vez cuando entra en zona crítica
};

#endif // SENSOR_MANAGER_H
