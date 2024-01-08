#ifndef MqttModule_h
#define MqttModule_h

#include <PubSubClient.h>

extern const int LED_CONNECTION;
extern const int LED_NOT_CONNECTION;

class MqttModule
{
public:
    static void conectarMQTT(PubSubClient &mqttClient, const char *server, int port);
    static void enviarMensajeMQTT(PubSubClient &mqttClient, const String &mensaje, const String &topic);
};

#endif
