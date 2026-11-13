# TP02 Integrador - Tecnologías para la Automatización (TA) 4k01/26
## Universidad Tecnológica Nacional - Facultad Regional Córdoba (UTN-FRC)

Sistema embebido basado en **ESP32** para supervisión ambiental, control de actuadores, telemetría IoT mediante **ThingSpeak** e interfaz de control interactivo mediante **Bot de Telegram** con botones en pantalla (Inline Keyboard).

---

## 🛠 Arquitectura de Hardware y Pines (Placa UTN Evaluation Board V1.0)

| Componente | Pin ESP32 (GPIO) | Tipo | Función |
| :--- | :--- | :--- | :--- |
| **Sensor DHT22** | `GPIO 33` | Digital | Medición de Temperatura y Humedad |
| **Potenciómetro** | `GPIO 32` | Analógico (ADC1_CH4) | Medición de tensión (0.0 V - 3.3 V) |
| **LED Verde** | `GPIO 23` | Digital Output | Actuador principal / Emulación de Relé |
| **LED Azul** | `GPIO 2` | Digital Output | LED de estado del sistema (Onboard) |
| **OLED SSD1306 (SDA)**| `GPIO 21` | I2C | Bus de datos para pantalla OLED 128x64 |
| **OLED SSD1306 (SCL)**| `GPIO 22` | I2C | Bus de reloj para pantalla OLED 128x64 |

---

## 📦 Estructura Modular y Librerías Propias

Para cumplir con la consigna de **código limpio, no bloqueante y delegación en librerías propias**, el `loop()` principal sólo contiene llamadas periódicas de actualización:

- **`Config.h`**: Mapeo centralizado de pines, credenciales Wi-Fi, Telegram, ThingSpeak y constantes de temporización.
- **`DisplayManager (.h / .cpp)`**: Control del display OLED I2C, pantalla de inicio, visualización de estados y manejo de errores de sintaxis en comandos.
- **`SensorManager (.h / .cpp)`**: Lectura no bloqueante del DHT22, promediado de muestras ADC del potenciómetro, conversión a voltios y control de histéresis para disparar alertas de seguridad automáticas.
- **`ActuatorManager (.h / .cpp)`**: Control no bloqueante del relé / actuador mediante `millis()`, apagado automático tras expirar el tiempo programado.
- **`ThingSpeakManager (.h / .cpp)`**: Envío periódico (HTTP GET) y manual de variables hacia los Fields 1, 2 y 3 de ThingSpeak.
- **`TelegramManager (.h / .cpp)`**: Manejo del bot con `UniversalTelegramBot`, generación de teclado interactivo (Inline Keyboard), parser de comandos y despacho de alertas críticas proactivas.

---

## 🤖 Comandos del Bot de Telegram

1. **`/start`**: Despliega el menú interactivo con botones Inline (sin necesidad de escribir comandos a mano):
   - *[ 🌡 Ver Clima ]* -> Ejecuta `/clima`
   - *[ 📊 Potenciómetro ]* -> Ejecuta `/thingup`
   - *[ ⚡ Relé (5s) ]* -> Ejecuta `/rele on 5`
   - *[ 🛑 Relé OFF ]* -> Ejecuta `/rele off`
   - *[ 🖥 Display LED ]* -> Ejecuta `/displayled`
   - *[ 🖥 Display Clima ]* -> Ejecuta `/displaydht`
   - *[ 🖥 Display Pot ]* -> Ejecuta `/displaypot`
2. **`/clima`**: Devuelve los últimos valores registrados de temperatura (°C) y humedad relativa (%) del sensor DHT22.
3. **`/rele <on/off> [segundos]`**: Conmuta el relé. Ej: `/rele on 5` (enciende por 5 segundos y se apaga automáticamente de forma no bloqueante).
4. **`/thingup`**: Lee el voltaje actual del potenciómetro, lo envía a ThingSpeak, responde el estado al usuario y verifica si superó el umbral crítico (2.80 V).
5. **Alerta automática de seguridad**: Si el potenciómetro supera el umbral crítico (2.80 V), la ESP32 envía un mensaje de alarma de inmediato al chat sin intervención del usuario.
6. **`/display<cmd>`**:
   - `/displayled`: Muestra el estado del LED/Relé en la pantalla OLED.
   - `/displaypot`: Muestra el voltaje y ADC del potenciómetro en la pantalla OLED.
   - `/displaydht`: Muestra la temperatura y humedad en la pantalla OLED.
   - `/display<invalido>`: Si el comando no es reconocido (ej. `/displaymotor`), **lo informa explícitamente en la pantalla OLED**.
