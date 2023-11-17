#ifndef HttpModule_h
#define HttpModule_h

#include <Arduino.h>

class HttpModule
{
public:
    HttpModule(const char* server, int http_port);
    void enviarDatosHTTP(const char *jsonString, const char *path);
    void activarActuador(const char *path, const char *idActuador);
private:
    const char *server;
    int http_port;
    void estructuraHttp(const char *path, const char *tipoSolicitud, const char* jsonString);
};
#endif