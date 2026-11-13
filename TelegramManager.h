#ifndef TELEGRAM_MANAGER_H
#define TELEGRAM_MANAGER_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "Config.h"
#include "DisplayManager.h"
#include "SensorManager.h"
#include "ActuatorManager.h"
#include "ThingSpeakManager.h"

class TelegramManager {
private:
    WiFiClientSecure securedClient;
    UniversalTelegramBot bot;

    DisplayManager& display;
    SensorManager& sensors;
    ActuatorManager& actuators;
    ThingSpeakManager& thingspeak;

    unsigned long lastCheckTime;

    void handleNewMessages(int numNewMessages);
    void processCommand(const String& chat_id, const String& from_name, const String& text);
    void sendStartMenu(const String& chat_id);
    void parseRelayCommand(const String& chat_id, const String& text);
    void parseDisplayCommand(const String& chat_id, const String& text);

public:
    TelegramManager(DisplayManager& disp, SensorManager& sens, 
                    ActuatorManager& act, ThingSpeakManager& ts);
    void begin();
    void update();

    // Envío proactivo de alerta de seguridad
    void sendSecurityAlert(float voltage);
};

#endif // TELEGRAM_MANAGER_H
