#include <WiFi.h>
#include <HTTPClient.h>
#include <Preferences.h>

// Crear objeto preferences
Preferences preferences;

// Variables para WiFi
char ssid[33] = "";      // SSID máximo 32 caracteres + null
char password[65] = "";  // Password máximo 64 caracteres + null
const char* apiUrl = "https://webhook.site/cd007f65-68b0-4aa9-bf59-ce8c0d3d5881";

// Pines UART
#define RX_PIN 16
#define TX_PIN 17

// Función para leer desde Serial
void readCredential(char* buffer, size_t maxLen) {
  size_t index = 0;
  while (index < maxLen - 1) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\n') break;
      if (c != '\r') buffer[index++] = c;
    }
  }
  buffer[index] = '\0';
}

// Función para conectar WiFi
bool connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  for (int i = 0; i < 20; i++) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConectado!");
      return true;
    }
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nFallo la conexión");
  return false;
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
  
  // Iniciar preferences
  preferences.begin("wifi", false);
  
  // Intentar leer credenciales guardadas
  preferences.getString("ssid", ssid, sizeof(ssid));
  preferences.getString("pass", password, sizeof(password));
  
  // Si no hay credenciales o falla la conexión, pedir nuevas
  if (strlen(ssid) == 0 || strlen(password) == 0 || !connectWiFi()) {
    Serial.println("\nIngrese SSID:");
    while (!Serial.available()) delay(100);
    readCredential(ssid, sizeof(ssid));
    
    Serial.println("Ingrese Password:");
    while (!Serial.available()) delay(100);
    readCredential(password, sizeof(password));
    
    // Guardar nuevas credenciales
    preferences.putString("ssid", ssid);
    preferences.putString("pass", password);
    
    // Intentar conectar con nuevas credenciales
    if (!connectWiFi()) {
      Serial.println("No se pudo conectar. Reiniciando...");
      ESP.restart();
    }
  }
  
  preferences.end();
  Serial.println("WiFi conectado");
}

void loop() {
  if (Serial2.available()) {
    String receivedData = Serial2.readStringUntil('\n');
    receivedData.trim();

    if (receivedData.length() > 0) {
      Serial.println("Datos: " + receivedData);

      int hIdx = receivedData.indexOf("Hum:");
      int tIdx = receivedData.indexOf("Tem:");
      if (hIdx != -1 && tIdx != -1) {
        float hum = receivedData.substring(hIdx + 4, tIdx).toFloat();
        float temp = receivedData.substring(tIdx + 4).toFloat();

        String json = "{\"humedad\":" + String(hum, 2) + ",\"temperatura\":" + String(temp, 2) + "}";

        if (WiFi.status() == WL_CONNECTED) {
          HTTPClient http;
          http.begin(apiUrl);
          http.addHeader("Content-Type", "application/json");
          int httpResponseCode = http.POST(json);
          Serial.println("HTTP Response: " + String(httpResponseCode));
          http.end();
        }
      }
    }
  }
  delay(100);
}