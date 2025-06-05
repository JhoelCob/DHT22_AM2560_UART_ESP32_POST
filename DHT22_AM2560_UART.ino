#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT22   

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  dht.begin();
}

void loop() {
  float hum = dht.readHumidity();
  float tem = dht.readTemperature();

  if (isnan(hum) || isnan(tem)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
  
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(tem);
  Serial.println(" *C");

    // Enviar datos al ESP32
    String data = "Humidity: " + String(hum) + " Temperature: " + String(tem) + " C";  
    Serial1.println(data);
  
    delay(15000);

}

