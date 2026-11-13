#include "ThingSpeakManager.h"

ThingSpeakManager::ThingSpeakManager()
    : lastSendTime(0),
      lastResponseCode(0),
      lastSendSuccess(false) {}

void ThingSpeakManager::begin() {
    lastSendTime = millis() - THINGSPEAK_SEND_INTERVAL_MS + 5000; // Primer envío tras 5s de arranque
    Serial.println("[ThingSpeak] ThingSpeakManager inicializado.");
}

bool ThingSpeakManager::sendData(float temp, float hum, float potVoltage) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[ThingSpeak] Error: Wi-Fi desconectado.");
        lastSendSuccess = false;
        return false;
    }

    // Validación básica de API Key configurada
    if (String(THINGSPEAK_API_KEY) == "TU_WRITE_API_KEY_AQUI") {
        Serial.println("[ThingSpeak] Aviso: API Key por defecto. Configure THINGSPEAK_API_KEY en Config.h.");
    }

    WiFiClient client;
    HTTPClient http;

    // Construcción de la URL de actualización HTTP GET
    // Field1: Temperatura, Field2: Humedad, Field3: Voltaje Potenciómetro
    String url = "http://";
    url += THINGSPEAK_SERVER;
    url += "/update?api_key=";
    url += THINGSPEAK_API_KEY;
    url += "&field1=" + String(temp, 2);
    url += "&field2=" + String(hum, 2);
    url += "&field3=" + String(potVoltage, 2);

    http.begin(client, url);
    http.setTimeout(3000); // 3 segundos maximo de espera para no trabar el flujo
    int httpCode = http.GET();
    lastResponseCode = httpCode;

    if (httpCode > 0) {
        String payload = http.getString();
        // ThingSpeak devuelve el Entry ID (> 0) o 0 en caso de rate-limit
        if (httpCode == HTTP_CODE_OK && payload.toInt() > 0) {
            Serial.printf("[ThingSpeak] Envio OK. Entrada #%s | T:%.1f H:%.1f V:%.2f\n", 
                          payload.c_str(), temp, hum, potVoltage);
            lastSendSuccess = true;
        } else {
            Serial.printf("[ThingSpeak] Respuesta: %d, Payload: %s (posible rate limit de 15s)\n", 
                          httpCode, payload.c_str());
            lastSendSuccess = false;
        }
    } else {
        Serial.printf("[ThingSpeak] Error de conexion HTTP: %s\n", http.errorToString(httpCode).c_str());
        lastSendSuccess = false;
    }

    http.end();
    lastSendTime = millis();
    return lastSendSuccess;
}

void ThingSpeakManager::update(float temp, float hum, float potVoltage) {
    unsigned long now = millis();
    if (now - lastSendTime >= THINGSPEAK_SEND_INTERVAL_MS) {
        sendData(temp, hum, potVoltage);
    }
}
