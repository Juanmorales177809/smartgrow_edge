#include "httpModule.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

void HttpModule::enviarDatosHTTP(const char* server, const int http_port, const char* jsonString) {
    HTTPClient http;
    Serial.println("[HTTP] Iniciando ... ");
    http.begin("http://" + String(server) + ":" + String(http_port) + "/phec");
    http.addHeader("Content-Type", "application/json");
    Serial.println("[HTTP] POST...");
    Serial.println(jsonString);
    int httpCode = http.POST(jsonString);
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    http.end();
}
