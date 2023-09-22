#ifndef WiFiModule_h
#define WiFiModule_h

#include <WiFi.h>

class WiFiModule {
public:
    static void conectarWiFi(const char* ssid, const char* password);
};

#endif

