#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  // Aguarda até a serial estar pronta (importante no C3)
  while (!Serial) {
    delay(10);
  }

  delay(2000); // dá tempo pro chip inicializar

  String mac = WiFi.macAddress();
  Serial.println("Endereço MAC do ESP32-C3 (Wi-Fi):");
  Serial.println(mac);
}

void loop() {
  // imprime a cada 5 segundos
  delay(5000);
  Serial.println(WiFi.macAddress());
}
