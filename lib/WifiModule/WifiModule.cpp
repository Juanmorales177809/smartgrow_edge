#include "WifiModule.h"

WifiModule::WifiModule(const char *ssid,const char *password){
    this->ssid = ssid;
    this->password = password;
}

void WifiModule::conectarWifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(this->ssid, this->password);
    Serial.println("Conectando a WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.println(".");
        delay(500);
    }
    Serial.println("WiFi Conectado");
    Serial.println("Dirección IP:");
    Serial.println(WiFi.localIP());
}