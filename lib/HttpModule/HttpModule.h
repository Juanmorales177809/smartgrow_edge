#ifndef HttpModule_h
#define HttpModule_h

#include <Arduino.h>

class HttpModule {
public:
    static void enviarDatosHTTP(const char* server, const int http_port, const char* jsonString);
};

#endif