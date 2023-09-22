#include "WiFiModule.h"

void WiFiModule::conectarWiFi(const char* ssid, const char* password) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("Conectando a WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.println(".");
        delay(500);
    }
    Serial.println("WiFi Conectado");
    Serial.println("Dirección IP:");
    Serial.println(WiFi.localIP());
}