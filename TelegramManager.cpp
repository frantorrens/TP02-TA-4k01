#include "TelegramManager.h"

TelegramManager::TelegramManager(DisplayManager& disp, SensorManager& sens, 
                                 ActuatorManager& act, ThingSpeakManager& ts)
    : securedClient(),
      bot(BOT_TOKEN, securedClient),
      display(disp),
      sensors(sens),
      actuators(act),
      thingspeak(ts),
      lastCheckTime(0) {}

void TelegramManager::begin() {
    securedClient.setInsecure(); // Permite conexiones SSL/TLS sin validar CA (ideal para ESP32)
    securedClient.setTimeout(3); // Timeout de 3 seg (evita bloqueos de 120 seg)
    bot.maxMessageLength = 4096; // Buffer ampliado para que NUNCA trunque el JSON de callback_query
    bot.waitForResponse = 500;   // Lectura rápida de respuestas HTTP de Telegram
    bot.longPoll = 0;           // Polling inmediato sin retención de conexión en el servidor
    Serial.println("[Telegram] TelegramManager inicializado.");
}

void TelegramManager::sendStartMenu(const String& chat_id) {
    // 1. /start - Devuelve un teclado inline interactivo compacto y robusto
    String keyboardJson = "["
        "[{\"text\":\"🌡 Ver Clima\",\"callback_data\":\"/clima\"},"
        " {\"text\":\"📊 Potenciometro\",\"callback_data\":\"/thingup\"}],"
        "[{\"text\":\"⚡ Rele (5s)\",\"callback_data\":\"/rele on 5\"},"
        " {\"text\":\"🛑 Rele OFF\",\"callback_data\":\"/rele off\"}],"
        "[{\"text\":\"🖥 Display LED\",\"callback_data\":\"/displayled\"},"
        " {\"text\":\"🖥 Display DHT\",\"callback_data\":\"/displaydht\"}],"
        "[{\"text\":\"🖥 Display Pot\",\"callback_data\":\"/displaypot\"}]"
    "]";

    String welcomeMsg = "⚙️ *Panel de Control ESP32 (UTN FRC - 4k01)*\nSeleccione una opción:";
    bot.sendMessageWithInlineKeyboard(chat_id, welcomeMsg, "Markdown", keyboardJson);
}

void TelegramManager::parseRelayCommand(const String& chat_id, const String& text) {
    // 3. /rele<on/off> <time>
    // Soporta: "/rele on 5", "/releon 5", "/rele on", "/rele off", "/releoff"
    String cleanText = text;
    cleanText.toLowerCase();
    cleanText.trim();

    if (cleanText.indexOf("off") >= 0) {
        actuators.turnRelayOff();
        bot.sendMessage(chat_id, "🛑 *Relé Desactivado*\nSalida digital apagada correctamente.", "Markdown");
        return;
    }

    if (cleanText.indexOf("on") >= 0) {
        // Extraer segundos si se proveyeron
        unsigned long seconds = RELE_DEFAULT_TIME_SECONDS;
        
        // Buscar el último número en el texto
        int lastSpace = cleanText.lastIndexOf(' ');
        if (lastSpace >= 0) {
            String timeStr = cleanText.substring(lastSpace + 1);
            int parsedTime = timeStr.toInt();
            if (parsedTime > 0) {
                seconds = (unsigned long)parsedTime;
            }
        } else {
            // Caso donde escriben "/releon5"
            int indexOn = cleanText.indexOf("on");
            if (indexOn >= 0 && cleanText.length() > (unsigned int)(indexOn + 2)) {
                String sub = cleanText.substring(indexOn + 2);
                sub.trim();
                int parsed = sub.toInt();
                if (parsed > 0) {
                    seconds = (unsigned long)parsed;
                }
            }
        }

        actuators.turnRelayOn(seconds);
        String resp = "⚡ *Relé Activado*\n";
        resp += "• Salida digital encendida por *" + String(seconds) + " segundos*.\n";
        resp += "• Se apagara automaticamente usando temporizador no bloqueante.";
        bot.sendMessage(chat_id, resp, "Markdown");
        return;
    }

    // Sintaxis no reconocida para el relé
    String helpMsg = "ℹ️ *Sintaxis incorrecta para el rele:*\n";
    helpMsg += "Ejemplos validos:\n";
    helpMsg += "• `/rele on 5` (enciende por 5 seg)\n";
    helpMsg += "• `/rele off` (apaga inmediatamente)";
    bot.sendMessage(chat_id, helpMsg, "Markdown");
}

void TelegramManager::parseDisplayCommand(const String& chat_id, const String& text) {
    // 5. /display<cmd>
    // Muestra en OLED: /displayled, /displaypot, /displaydht (o /displayclima),
    // o cualquier otro comando no identificado informando en pantalla.
    String cmd = text;
    cmd.toLowerCase();
    cmd.trim();

    if (cmd == "/displayled") {
        display.showLedState(actuators.isGreenLedOn(), actuators.isBlueLedOn());
        String msg = "🖥 *Display OLED Actualizado*\n";
        msg += "Mostrando estado de LEDs en pantalla:\n";
        msg += "• Verde (Relé): " + String(actuators.isGreenLedOn() ? "ENCENDIDO" : "APAGADO") + "\n";
        msg += "• Azul: " + String(actuators.isBlueLedOn() ? "ENCENDIDO" : "APAGADO");
        bot.sendMessage(chat_id, msg, "Markdown");
    } 
    else if (cmd == "/displaypot") {
        display.showPotState(sensors.getPotVoltage(), sensors.getPotRaw());
        String msg = "🖥 *Display OLED Actualizado*\n";
        msg += "Mostrando potenciómetro:\n";
        msg += "• Voltaje: " + String(sensors.getPotVoltage(), 2) + " V\n";
        msg += "• ADC: " + String(sensors.getPotRaw()) + " / 4095";
        bot.sendMessage(chat_id, msg, "Markdown");
    } 
    else if (cmd == "/displaydht" || cmd == "/displayclima" || cmd == "/displaytemp") {
        display.showDhtState(sensors.getTemperature(), sensors.getHumidity());
        String msg = "🖥 *Display OLED Actualizado*\n";
        msg += "Mostrando DHT22:\n";
        msg += "• Temp: " + String(sensors.getTemperature(), 1) + " °C\n";
        msg += "• Humedad: " + String(sensors.getHumidity(), 1) + " %";
        bot.sendMessage(chat_id, msg, "Markdown");
    } 
    else {
        // Requerimiento clave: comando no identificado informado en la pantalla OLED
        display.showUnknownCommand(text);
        String msg = "⚠️ *Comando no identificado para el display:* `" + text + "`\n";
        msg += "Esta situacion ha sido informada en la pantalla OLED de la placa.";
        bot.sendMessage(chat_id, msg, "Markdown");
    }
}

void TelegramManager::processCommand(const String& chat_id, const String& from_name, const String& text) {
    Serial.printf("[Telegram] Mensaje de %s (%s): %s\n", from_name.c_str(), chat_id.c_str(), text.c_str());

    String cleanText = text;
    cleanText.trim();

    if (cleanText == "/start") {
        sendStartMenu(chat_id);
    }
    else if (cleanText == "/clima") {
        // 2. /clima - Devuelve ultimos valores registrados de temp y hum del DHT22
        String resp = "🌡 *Condiciones Ambientales (DHT22)*\n\n";
        resp += "• Temperatura: *" + String(sensors.getTemperature(), 1) + " °C*\n";
        resp += "• Humedad Relativa: *" + String(sensors.getHumidity(), 1) + " %*";
        bot.sendMessage(chat_id, resp, "Markdown");
    }
    else if (cleanText.startsWith("/rele")) {
        parseRelayCommand(chat_id, cleanText);
    }
    else if (cleanText == "/thingup") {
        // 4. /thingup - Enviar valor del potenciometro (voltaje) a ThingSpeak y evaluar umbral critico
        sensors.forceReadPot();
        float voltage = sensors.getPotVoltage();
        int rawAdc = sensors.getPotRaw();

        bool tsOk = thingspeak.sendData(sensors.getTemperature(), sensors.getHumidity(), voltage);

        String resp = "📊 *Actualizacion ThingSpeak (/thingup)*\n\n";
        resp += "• Voltaje Potenciometro: *" + String(voltage, 2) + " V*\n";
        resp += "• Valor Raw ADC: *" + String(rawAdc) + " / 4095*\n";
        resp += "• Envio ThingSpeak: " + String(tsOk ? "✅ Exitoso" : "⚠️ Fallido / En espera (15s)") + "\n";

        if (voltage >= POT_VOLTAGE_THRESHOLD_CRITICAL) {
            resp += "\n🚨 *ESTADO CRÍTICO:* El voltaje (";
            resp += String(voltage, 2);
            resp += " V) supero el umbral permitido (";
            resp += String(POT_VOLTAGE_THRESHOLD_CRITICAL, 2);
            resp += " V)!";
        } else {
            resp += "\n✅ *ESTADO NORMAL:* Voltaje dentro de rangos operativos seguros.";
        }

        bot.sendMessage(chat_id, resp, "Markdown");
    }
    else if (cleanText.startsWith("/display")) {
        parseDisplayCommand(chat_id, cleanText);
    }
    else {
        String msg = "❓ Comando no reconocido: `" + cleanText + "`\n";
        msg += "Presione /start para abrir el panel de control.";
        bot.sendMessage(chat_id, msg, "Markdown");
    }
}

void TelegramManager::handleNewMessages(int numNewMessages) {
    for (int i = 0; i < numNewMessages; i++) {
        // En los botones inline (callback_query), el chat_id viene en chat_id o en from_id:
        String chat_id = String(bot.messages[i].chat_id);
        if (chat_id.length() == 0 || chat_id == "0") {
            chat_id = String(bot.messages[i].from_id);
        }
        if (chat_id.length() == 0 || chat_id == "0") {
            chat_id = String(CHAT_ID);
        }

        String text = bot.messages[i].text;
        String from_name = bot.messages[i].from_name;

        Serial.printf("[Telegram DEBUG] Tipo: %s | chat_id: %s | text: %s\n", 
                      bot.messages[i].type.c_str(), chat_id.c_str(), text.c_str());

        // Asegurar que el socket SSL esté limpio antes de enviar respuesta
        securedClient.stop();

        processCommand(chat_id, from_name, text);
    }
}

void TelegramManager::sendSecurityAlert(float voltage) {
    // Disparo automatico de alerta sin que el usuario lo pida
    String alertMsg = "🚨🚨🚨 *¡ALERTA DE SEGURIDAD CRÍTICA!* 🚨🚨🚨\n\n";
    alertMsg += "⚠️ El voltaje del potenciometro supero el limite seguro!\n";
    alertMsg += "• Voltaje detectado: *" + String(voltage, 2) + " V*\n";
    alertMsg += "• Umbral maximo: *" + String(POT_VOLTAGE_THRESHOLD_CRITICAL, 2) + " V*\n\n";
    alertMsg += "🔔 *Accion sugerida:* Inspeccione el potenciometro y verifique la salida.";

    // Se envia al CHAT_ID configurado
    if (String(CHAT_ID) != "TU_CHAT_ID_AQUI") {
        bot.sendMessage(CHAT_ID, alertMsg, "Markdown");
        Serial.printf("[Telegram] Alerta de seguridad enviada al chat %s\n", CHAT_ID);
    } else {
        Serial.println("[Telegram] Alerta detectada, pero CHAT_ID no esta configurado en Config.h.");
    }
}

void TelegramManager::update() {
    // 1. Chequeo proactivo de evento de alerta critica sin esperar comando
    if (sensors.takeCriticalAlertEvent()) {
        sendSecurityAlert(sensors.getPotVoltage());
    }

    // 2. Sondeo no bloqueante de nuevos mensajes y botones inline
    unsigned long now = millis();
    if (now - lastCheckTime >= TELEGRAM_POLL_INTERVAL_MS) {
        if (WiFi.status() == WL_CONNECTED && String(BOT_TOKEN) != "TU_BOT_TOKEN_AQUI") {
            int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
            if (numNewMessages > 0) {
                handleNewMessages(numNewMessages);
            }
        }
        lastCheckTime = millis(); // Registrar marca al terminar para mantener intervalo estable
    }
}
