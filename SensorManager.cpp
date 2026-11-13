#include "SensorManager.h"

SensorManager::SensorManager() 
    : dht(PIN_DHT, DHT_TYPE),
      currentTemperature(0.0f),
      currentHumidity(0.0f),
      currentPotVoltage(0.0f),
      currentPotRaw(0),
      lastReadTime(0),
      criticalAlertPending(false),
      criticalAlertActive(false) {}

void SensorManager::begin() {
    pinMode(PIN_POT, INPUT);
    analogReadResolution(12); // ESP32 ADC de 12 bits (0 a 4095)
    
    dht.begin();
    delay(50); // Breve espera inicial para estabilización del bus DHT
    readSensors();
    Serial.println("[Sensores] SensorManager inicializado.");
}

void SensorManager::readSensors() {
    // 1. Lectura del DHT22
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (!isnan(t)) {
        currentTemperature = t;
    } else {
        Serial.println("[Sensores] Error al leer temperatura del DHT22.");
    }

    if (!isnan(h)) {
        currentHumidity = h;
    } else {
        Serial.println("[Sensores] Error al leer humedad del DHT22.");
    }

    // 2. Lectura del potenciómetro con promediado de 8 muestras para reducir ruido
    long rawSum = 0;
    const int samples = 8;
    for (int i = 0; i < samples; i++) {
        rawSum += analogRead(PIN_POT);
        delayMicroseconds(50);
    }
    currentPotRaw = rawSum / samples;

    // Cálculo del voltaje (ADC de 12 bits: 0 a 4095 -> 0.0V a 3.3V)
    currentPotVoltage = ((float)currentPotRaw / 4095.0f) * 3.3f;

    // 3. Evaluación de umbral crítico con histéresis
    if (currentPotVoltage >= POT_VOLTAGE_THRESHOLD_CRITICAL) {
        if (!criticalAlertActive) {
            criticalAlertActive = true;
            criticalAlertPending = true; // Se dispara el evento de alerta para Telegram
            Serial.printf("[Alerta] Potenciometro supero umbral critico: %.2f V\n", currentPotVoltage);
        }
    } else if (currentPotVoltage <= (POT_VOLTAGE_THRESHOLD_CRITICAL - POT_VOLTAGE_HYSTERESIS)) {
        // Al descender por debajo del margen de histéresis se re-arma la alarma
        if (criticalAlertActive) {
            criticalAlertActive = false;
            Serial.printf("[Alerta] Potenciometro normalizado: %.2f V\n", currentPotVoltage);
        }
    }
}

void SensorManager::update() {
    unsigned long now = millis();
    if (now - lastReadTime >= SENSOR_READ_INTERVAL_MS) {
        lastReadTime = now;
        readSensors();
    }
}

void SensorManager::forceReadPot() {
    // Lectura forzada para actualización inmediata (ej: /thingup)
    long rawSum = 0;
    const int samples = 8;
    for (int i = 0; i < samples; i++) {
        rawSum += analogRead(PIN_POT);
        delayMicroseconds(50);
    }
    currentPotRaw = rawSum / samples;
    currentPotVoltage = ((float)currentPotRaw / 4095.0f) * 3.3f;

    if (currentPotVoltage >= POT_VOLTAGE_THRESHOLD_CRITICAL && !criticalAlertActive) {
        criticalAlertActive = true;
        criticalAlertPending = true;
    }
}

bool SensorManager::isPotVoltageCritical() const {
    return currentPotVoltage >= POT_VOLTAGE_THRESHOLD_CRITICAL;
}

bool SensorManager::takeCriticalAlertEvent() {
    if (criticalAlertPending) {
        criticalAlertPending = false;
        return true;
    }
    return false;
}
