// =============================================================================
// TRABAJO PRÁCTICO INTEGRADOR - TECNOLOGÍAS PARA LA AUTOMATIZACIÓN (TA) 4k01/26
// Universidad Tecnológica Nacional - Facultad Regional Córdoba (UTN-FRC)
// 
// Sistema Embebido ESP32: Supervisión Ambiental (DHT22), Potenciómetro (ADC),
// Display OLED I2C, Actuación Remota (Relé/LEDs), ThingSpeak IoT y Telegram Bot.
// =============================================================================

#include <Arduino.h>
#include <WiFi.h>

#include "Config.h"
#include "DisplayManager.h"
#include "SensorManager.h"
#include "ActuatorManager.h"
#include "ThingSpeakManager.h"
#include "TelegramManager.h"

// -----------------------------------------------------------------------------
// INSTANCIAS DE LIBRERÍAS PROPIAS
// -----------------------------------------------------------------------------
DisplayManager display;
SensorManager sensors;
ActuatorManager actuators;
ThingSpeakManager thingspeak;
TelegramManager telegram(display, sensors, actuators, thingspeak);

// -----------------------------------------------------------------------------
// INICIALIZACIÓN DE CONECTIVIDAD WI-FI
// -----------------------------------------------------------------------------
void setupWiFi() {
    Serial.printf("\n[WiFi] Conectando a red: %s", WIFI_SSID);
    display.showWiFiStatus("Conectando...");

    WiFi.mode(WIFI_STA);

    // En Wokwi usamos canal 6 para conexión instantánea; en la placa física escanea todos los canales automáticamente
    if (String(WIFI_SSID) == "Wokwi-GUEST") {
        WiFi.begin(WIFI_SSID, WIFI_PASS, 6);
    } else {
        WiFi.begin(WIFI_SSID, WIFI_PASS);
    }

    // Espera a que termine la asignación de IP
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }

    Serial.printf("\n[WiFi] Conectado exitosamente. IP: %s\n", WiFi.localIP().toString().c_str());
    WiFi.setSleep(false); // Desactiva reposo del modem para respuesta instantanea en Telegram
    actuators.setBlueLed(true); // Enciende LED azul indicando que el sistema está ONLINE y listo
    display.showWiFiStatus("Conectado OK", WiFi.localIP().toString());
}

// -----------------------------------------------------------------------------
// SETUP
// -----------------------------------------------------------------------------
void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n==============================================");
    Serial.println("  UTN FRC - Tecnologías para la Automatización");
    Serial.println("  TP02: Sistema IoT ESP32 + Telegram + ThingSpeak");
    Serial.println("==============================================\n");

    // Inicializar módulos propios
    display.begin();
    actuators.begin();
    sensors.begin();
    thingspeak.begin();

    // Conectar a la red
    setupWiFi();

    // Inicializar servicio de Telegram
    telegram.begin();

    delay(1500); // Pausa breve para lectura de pantalla de bienvenida en OLED
    display.showStatus("Sistema Activo", "Esperando comandos", "Telegram Bot");
}

// -----------------------------------------------------------------------------
// LOOP PRINCIPAL (Código mínimo - toda la lógica delegada a librerías propias)
// -----------------------------------------------------------------------------
void loop() {
    // Verificación y reconexión automática si se pierde Wi-Fi
    if (WiFi.status() != WL_CONNECTED) {
        actuators.setBlueLed(false); // LED Azul apagado si se cae la red
        static unsigned long lastReconnectAttempt = 0;
        if (millis() - lastReconnectAttempt > 10000) {
            lastReconnectAttempt = millis();
            Serial.println("[WiFi] Reintentando reconexion automatica...");
            WiFi.reconnect();
        }
    } else {
        actuators.setBlueLed(true);  // LED Azul encendido fijo como testigo Online
    }

    // 1. Actualización de temporizador no bloqueante del relé con millis()
    actuators.update();

    // 2. Muestreo periódico de sensores (DHT22 y Potenciómetro con umbral crítico)
    sensors.update();

    // 3. Envío periódico a ThingSpeak: si el relé está contando, espera a que termine para no demorarlo
    if (!actuators.isTimerActive()) {
        thingspeak.update(sensors.getTemperature(), 
                          sensors.getHumidity(), 
                          sensors.getPotVoltage());
    }

    // 4. Recepción de comandos Telegram, botones inline y alertas espontáneas
    telegram.update();
}
