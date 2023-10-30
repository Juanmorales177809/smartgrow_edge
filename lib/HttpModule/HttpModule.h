#ifndef HttpModule_h
#define HttpModule_h

#include <Arduino.h>

class HttpModule
{
public:
    static void enviarDatosHTTP(const char *server, const int http_port, const char *jsonString, const char *path);

    static void activarActuador(const char *server, const int http_port, const char *path, const char *idActuador);
};

void estructuraHttp(String server, const int http_port, String path, String tipoSolicitud,const char *jsonString);

#endif