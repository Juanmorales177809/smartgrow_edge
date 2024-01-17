#ifndef MqttModule_h
#define MqttModule_h

#include <PubSubClient.h>

extern int var;          // Declarar var como externa
extern String resultS;   // Declarar resultS como externa

class MqttModule {
public:
    static void conectarMQTT(PubSubClient& mqttClient, const char* server, int port);
    static void enviarMensajeMQTT(PubSubClient& mqttClient, const String& mensaje, const char* topic);
    static void callback(char* topic, byte* payload, unsigned int length);
};

#endif