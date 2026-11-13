#include "DisplayManager.h"

DisplayManager::DisplayManager() 
    : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1), initialized(false) {}

bool DisplayManager::begin() {
    Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
    
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
        Serial.println("[Display] Error al inicializar pantalla OLED SSD1306.");
        initialized = false;
        return false;
    }

    initialized = true;
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    showSplash();
    return true;
}

void DisplayManager::drawHeader(const char* title) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(title);
    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
    display.setCursor(0, 14);
}

void DisplayManager::showSplash() {
    if (!initialized) return;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(18, 5);
    display.println("UTN - FRC (TA)");
    display.setCursor(15, 20);
    display.println("4K01 / 2026");
    display.drawLine(10, 32, 118, 32, SSD1306_WHITE);
    display.setCursor(12, 38);
    display.println("TP02 INTEGRADOR");
    display.setCursor(20, 50);
    display.println("Iniciando...");
    display.display();
}

void DisplayManager::showWiFiStatus(const String& status, const String& ip) {
    if (!initialized) return;
    drawHeader("CONEXION WI-FI");
    display.setCursor(0, 16);
    display.println("Estado:");
    display.print(" ");
    display.println(status);
    
    if (ip.length() > 0) {
        display.setCursor(0, 38);
        display.println("IP Asignada:");
        display.print(" ");
        display.println(ip);
    }
    display.display();
}

void DisplayManager::showStatus(const String& line1, const String& line2, const String& line3) {
    if (!initialized) return;
    drawHeader("ESTADO GENERAL");
    display.setCursor(0, 16);
    display.println(line1);
    if (line2.length() > 0) {
        display.setCursor(0, 30);
        display.println(line2);
    }
    if (line3.length() > 0) {
        display.setCursor(0, 44);
        display.println(line3);
    }
    display.display();
}

// -----------------------------------------------------------------------------
// Comando: /displayled
// Muestra: "Led encendido/apagado" según corresponda
// -----------------------------------------------------------------------------
void DisplayManager::showLedState(bool greenOn, bool blueOn) {
    if (!initialized) return;
    drawHeader("ESTADO DE SALIDAS");
    
    display.setCursor(0, 16);
    display.print("LED Verde (Rele): ");
    display.setCursor(10, 28);
    display.setTextSize(1);
    if (greenOn) {
        display.println("[ ENCENDIDO ]");
    } else {
        display.println("[ APAGADO ]");
    }

    display.setCursor(0, 42);
    display.print("LED Azul (Online): ");
    display.setCursor(10, 52);
    if (blueOn) {
        display.println("[ ENCENDIDO ]");
    } else {
        display.println("[ APAGADO ]");
    }

    display.display();
}

// -----------------------------------------------------------------------------
// Comando: /displaypot
// Muestra el voltaje actual y lectura ADC del potenciómetro
// -----------------------------------------------------------------------------
void DisplayManager::showPotState(float voltage, int rawAdc) {
    if (!initialized) return;
    drawHeader("POTENCIOMETRO (ADC)");
    
    display.setCursor(0, 16);
    display.print("Voltaje: ");
    display.print(voltage, 2);
    display.println(" V");

    display.setCursor(0, 30);
    display.print("ADC Raw: ");
    display.print(rawAdc);
    display.println(" / 4095");

    display.setCursor(0, 44);
    if (voltage >= POT_VOLTAGE_THRESHOLD_CRITICAL) {
        display.println("ALERTA: > UMBRAL!");
    } else {
        display.println("Estado: NORMAL");
    }

    display.display();
}

// -----------------------------------------------------------------------------
// Comando: /displaydht
// Muestra los valores de temperatura y humedad del sensor DHT22
// -----------------------------------------------------------------------------
void DisplayManager::showDhtState(float temp, float hum) {
    if (!initialized) return;
    drawHeader("SENSOR DHT22");
    
    display.setCursor(0, 16);
    display.print("Temperatura: ");
    display.print(temp, 1);
    display.println(" C");

    display.setCursor(0, 32);
    display.print("Humedad:     ");
    display.print(hum, 1);
    display.println(" %");

    display.setCursor(0, 48);
    display.println("Supervision OK");
    display.display();
}

// -----------------------------------------------------------------------------
// Comando no identificado: /display<invalido>
// Requerimiento: "En caso de recibir /display+comando no identificado informar
// esta situación en la pantalla."
// -----------------------------------------------------------------------------
void DisplayManager::showUnknownCommand(const String& cmd) {
    if (!initialized) return;
    drawHeader("! COMANDO INVALIDO !");
    
    display.setCursor(0, 16);
    display.println("Comando desconocido:");
    
    display.setCursor(0, 30);
    // Limitar longitud para no desbordar pantalla
    if (cmd.length() > 20) {
        display.println(cmd.substring(0, 20) + "...");
    } else {
        display.println(cmd);
    }

    display.setCursor(0, 46);
    display.println("Verifique sintaxis.");
    display.display();
}

void DisplayManager::clear() {
    if (!initialized) return;
    display.clearDisplay();
    display.display();
}
