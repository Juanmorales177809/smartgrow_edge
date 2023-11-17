#ifndef WifiModule_h
#define WifiModule_h

#include <WiFi.h>

class WifiModule
{
public:
    WifiModule(const char *ssid, const char *password);
    void conectarWifi();

private:
    const char *ssid;
    const char *password;
};

#endif
