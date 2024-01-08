#include "MqttModule.h"
#include "ActuadoresModule.h"

const int LED_CONNECTION = 19;
const int LED_NOT_CONNECTION = 18;

void MqttModule::conectarMQTT(PubSubClient &mqttClient, const char *server, int port)
{
    while (!mqttClient.connected())
    {
        Serial.print("Intentando conectarse MQTT...");
        if (mqttClient.connect("actuadoresClient"))
        {
            digitalWrite(LED_NOT_CONNECTION, LOW);
            digitalWrite(LED_CONNECTION, HIGH);
            Serial.println("Conectado");
            mqttClient.subscribe("smartgrow/hidroponico/actuadores");
        }
        else
        {
            digitalWrite(LED_CONNECTION, LOW);
            digitalWrite(LED_NOT_CONNECTION, HIGH);
            Serial.print("Fallo, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" intentar de nuevo en 5 segundos");
            delay(5000);
        }
    }
}

void MqttModule::enviarMensajeMQTT(PubSubClient &mqttClient, const String &mensaje, const String &topic)
{
    if (mqttClient.connected())
    {
        mqttClient.publish(topic.c_str(), mensaje.c_str());
    }
}
