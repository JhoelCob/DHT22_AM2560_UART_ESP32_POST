#include <WiFi.h>
#include <HTTPClient.h>

// Configuración WiFi
const char* ssid = "Zgroup_Piso1";
const char* password = "Zgr0up3361@@";
const char* apiUrl = "https://webhook.site/cd007f65-68b0-4aa9-bf59-ce8c0d3d5881";

// Pines UART
#define RX_PIN 16
#define TX_PIN 17

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");
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

        String json = "{\"Humedad\":" + String(hum, 2) + ",\"Temperatura\":" + String(temp, 2) + "}";

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