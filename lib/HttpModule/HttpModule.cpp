#include "HttpModule.h"
#include <HTTPClient.h>

HttpModule::HttpModule(const char* server, int http_port){
    this->server = server;
    this->http_port = http_port;
}

void HttpModule::enviarDatosHTTP(const char* jsonString, const char* path) {
    estructuraHttp(path, "POST", jsonString);
}

void HttpModule::activarActuador(const char* path, const char* idActuador) {
    const String full_path = String(path) + "?actuador=" + String(idActuador);
    estructuraHttp(full_path.c_str(), "GET", NULL);
}

void HttpModule::estructuraHttp(const char *path, const char *tipoSolicitud, const char* jsonString){
    HTTPClient http;
    Serial.println("[HTTP] Iniciando ... ");
    http.begin("http://" + String(this->server) + ":" + String(this->http_port) + "/" + path);
    http.addHeader("Content-Type", "application/json");
    Serial.println("[HTTP] " + String(tipoSolicitud) + "...");
    int httpCode;
    if (strcmp(tipoSolicitud,"GET") == 0) {
        httpCode = http.GET();
    } else if (strcmp(tipoSolicitud,"POST") == 0) {
        httpCode = http.POST(jsonString);
    }
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    http.end();
}