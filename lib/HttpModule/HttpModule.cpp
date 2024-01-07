#include "httpModule.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

void HttpModule::enviarDatosHTTP(const char* server, const int http_port, const char* jsonString, const char* path) {
    Serial.println(jsonString);
    estructuraHttp(server, http_port, path, "POST", jsonString);
}

void HttpModule::activarActuador(const char* server, const int http_port, const char* path, const char* idActuador) {
    const String full_path = String(path) + "?actuador=" + String(idActuador);
    estructuraHttp(server, http_port, full_path.c_str(), "GET", NULL);
}

void estructuraHttp(String server, const int http_port, String path, String tipoSolicitud,const char *jsonString ) {
    HTTPClient http;
    Serial.println("[HTTP] Iniciando ... ");
    http.begin("http://" + server + ":" + String(http_port) + "/" + path);
    http.addHeader("Content-Type", "application/json");
    Serial.println("[HTTP] " + String(tipoSolicitud) + "...");
    int httpCode;
    if (String(tipoSolicitud) == "GET") {
        httpCode = http.GET();
    } else if (String(tipoSolicitud) == "POST") {
        httpCode = http.POST(jsonString);
    }
    String payload = http.getString();
    Serial.print("Respuesta: ");
    Serial.println(httpCode);
    Serial.println(payload);
    http.end();
}


