#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// =============================================================================
// TRABAJO PRÁCTICO INTEGRADOR - TECNOLOGÍAS PARA LA AUTOMATIZACIÓN (TA) 4k01/26
// Universidad Tecnológica Nacional - Facultad Regional Córdoba (UTN-FRC)
// =============================================================================

// -----------------------------------------------------------------------------
// 1. CONFIGURACIÓN WI-FI
// -----------------------------------------------------------------------------
// En Wokwi se utiliza "Wokwi-GUEST" sin contraseña y en canal 6 (acelera la conexión virtual).
// Para la Placa UTN física, reemplace con el SSID y contraseña de su red/celular y ponga WIFI_CHANNEL en 0.
#define WIFI_SSID       "Wokwi-GUEST"
#define WIFI_PASS       ""
#define WIFI_CHANNEL    6 // Canal 6 obligatorio en Wokwi para conexión inmediata sin scan


// -----------------------------------------------------------------------------
// 2. CONFIGURACIÓN TELEGRAM BOT
// -----------------------------------------------------------------------------
// Obtenga el TOKEN creando un bot con @BotFather en Telegram.
// Obtenga su CHAT_ID personal hablándole a @userinfobot en Telegram.
#define BOT_TOKEN       "9097181268:AAHic7F51FUoEuH0ygdB_W3twuxs5enxVGk"
#define CHAT_ID         "5734115602"

// -----------------------------------------------------------------------------
// 3. CONFIGURACIÓN THINGSPEAK
// -----------------------------------------------------------------------------
// Obtenga su Write API Key en https://thingspeak.com/ -> Channels -> API Keys
#define THINGSPEAK_API_KEY      "2WEOAEHUYBJF0OLT"
#define THINGSPEAK_SERVER       "api.thingspeak.com"
#define THINGSPEAK_PORT         80

// -----------------------------------------------------------------------------
// 4. MAPEO DE PINES (GPIO) - ACORDE AL ENUNCIADO Y PLACA UTN EVALUATION BOARD V1.0
// -----------------------------------------------------------------------------
// DHT22 conectado a GPIO 33
#define PIN_DHT                 33
#define DHT_TYPE                DHT22

// Potenciómetro conectado al pin analógico GPIO 32 (ADC1_CH4)
#define PIN_POT                 32

// LED Verde al GPIO 23 (utilizado como relé/actuador principal según enunciado)
#define PIN_LED_VERDE           23

// LED Azul al GPIO 2 (LED testigo / onboard en ESP32)
#define PIN_LED_AZUL            2

// Definición de Relé / Actuador Principal:
// En la placa UTN se utiliza el LED Verde (GPIO 23) para emular la salida del Relé.
#define PIN_RELE                PIN_LED_VERDE

// Pantalla OLED I2C (SSD1306)
#define PIN_OLED_SDA            21
#define PIN_OLED_SCL            22
#define OLED_I2C_ADDRESS        0x3C
#define SCREEN_WIDTH            128
#define SCREEN_HEIGHT           64

// -----------------------------------------------------------------------------
// 5. PARÁMETROS OPERATIVOS Y TEMPORIZADORES (NO BLOQUEANTES)
// -----------------------------------------------------------------------------
// Intervalo de lectura periódica de sensores (ms)
#define SENSOR_READ_INTERVAL_MS         2000

// Intervalo de chequeo de mensajes en Telegram (ms)
#define TELEGRAM_POLL_INTERVAL_MS       1000

// Intervalo de envío periódico a ThingSpeak (ms) - Mínimo 15s permitido por ThingSpeak
#define THINGSPEAK_SEND_INTERVAL_MS     20000

// Umbral crítico de voltaje del potenciómetro para disparo de alerta (Voltios)
#define POT_VOLTAGE_THRESHOLD_CRITICAL  2.80f
#define POT_VOLTAGE_HYSTERESIS          0.20f // Margen para evitar rebotes de alerta

// Tiempo por defecto de activación del relé si no se especifica tiempo (segundos)
#define RELE_DEFAULT_TIME_SECONDS       5

#endif // CONFIG_H
