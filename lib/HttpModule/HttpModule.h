#ifndef HttpModule_h
#define HttpModule_h

#include <Arduino.h>

class HttpModule
{
public:
    HttpModule(const char *server, int http_port);
    static void enviarDatosHTTP(const char *server, const int http_port, const char *jsonString);
    static void enviarDatosActuadores(const char *server, const int http_port, const char *jsonString, const char *id);
};

#endif