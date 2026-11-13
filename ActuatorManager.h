#ifndef ACTUATOR_MANAGER_H
#define ACTUATOR_MANAGER_H

#include <Arduino.h>
#include "Config.h"

class ActuatorManager {
private:
    bool relayState;
    bool relayTimerActive;
    unsigned long relayStartTime;
    unsigned long relayDurationMs;

    bool blueLedState;

public:
    ActuatorManager();
    void begin();
    void update();

    // Control de relé con temporización no bloqueante mediante millis()
    void turnRelayOn(unsigned long seconds = 0); // 0 = permanente hasta que se apague
    void turnRelayOff();
    bool isRelayOn() const { return relayState; }
    bool isTimerActive() const { return relayTimerActive; }
    unsigned long getRemainingSeconds() const;

    // Control de LED azul de estado (Testigo Online / Wi-Fi)
    void setBlueLed(bool state);
    bool isBlueLedOn() const { return blueLedState; }

    // Estado para display
    bool isGreenLedOn() const { return relayState; }
};

#endif // ACTUATOR_MANAGER_H
