#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Config.h"

class DisplayManager {
private:
    Adafruit_SSD1306 display;
    bool initialized;

    void drawHeader(const char* title);

public:
    DisplayManager();
    bool begin();
    
    void showSplash();
    void showWiFiStatus(const String& status, const String& ip = "");
    void showStatus(const String& line1, const String& line2 = "", const String& line3 = "");
    
    // Métodos específicos solicitados por los comandos de Telegram
    void showLedState(bool greenOn, bool blueOn);
    void showPotState(float voltage, int rawAdc);
    void showDhtState(float temp, float hum);
    void showUnknownCommand(const String& cmd);
    
    void clear();
};

#endif // DISPLAY_MANAGER_H
