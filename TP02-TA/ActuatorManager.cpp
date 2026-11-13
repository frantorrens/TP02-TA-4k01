#include "ActuatorManager.h"

ActuatorManager::ActuatorManager()
    : relayState(false),
      relayTimerActive(false),
      relayStartTime(0),
      relayDurationMs(0),
      blueLedState(false) {}

void ActuatorManager::begin() {
    pinMode(PIN_LED_VERDE, OUTPUT);
    pinMode(PIN_LED_AZUL, OUTPUT);

    // Inicializar apagados
    digitalWrite(PIN_LED_VERDE, LOW);
    digitalWrite(PIN_LED_AZUL, LOW);

    relayState = false;
    blueLedState = false;
    relayTimerActive = false;

    Serial.println("[Actuadores] ActuatorManager inicializado.");
}

void ActuatorManager::turnRelayOn(unsigned long seconds) {
    relayState = true;
    digitalWrite(PIN_RELE, HIGH);
    relayStartTime = millis();

    if (seconds > 0) {
        relayTimerActive = true;
        relayDurationMs = seconds * 1000UL;
        Serial.printf("[Actuadores] Rele ENCENDIDO por %lu segundos.\n", seconds);
    } else {
        relayTimerActive = false; // Sin temporizador (permanente)
        Serial.println("[Actuadores] Rele ENCENDIDO permanente.");
    }
}

void ActuatorManager::turnRelayOff() {
    if (relayState) {
        relayState = false;
        relayTimerActive = false;
        digitalWrite(PIN_RELE, LOW);
        Serial.println("[Actuadores] Temporizador finalizado: Rele APAGADO.");
    } else {
        relayTimerActive = false;
        digitalWrite(PIN_RELE, LOW);
        Serial.println("[Actuadores] Rele APAGADO.");
    }
}

void ActuatorManager::update() {
    // Verificación clásica no bloqueante de la diferencia de tiempo con millis()
    if (relayTimerActive) {
        if (millis() - relayStartTime >= relayDurationMs) {
            turnRelayOff();
        }
    }
}

unsigned long ActuatorManager::getRemainingSeconds() const {
    if (!relayTimerActive || !relayState) return 0;
    unsigned long elapsed = millis() - relayStartTime;
    if (elapsed >= relayDurationMs) return 0;
    return (relayDurationMs - elapsed) / 1000UL;
}

void ActuatorManager::setBlueLed(bool state) {
    blueLedState = state;
    digitalWrite(PIN_LED_AZUL, state ? HIGH : LOW);
}
