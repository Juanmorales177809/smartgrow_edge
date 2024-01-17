#include "MqttModule.h"

int var = 0;         // Definir var globalmente
String resultS = ""; // Definir resultS globalmente

void MqttModule::conectarMQTT(PubSubClient &mqttClient, const char *server, int port)
{
    while (!mqttClient.connected())
    {
        Serial.print("Intentando conectarse MQTT...");
        if (mqttClient.connect("arduinoClient"))
        {
            Serial.println("Conectado");

            mqttClient.subscribe("smartgrow");
        }
        else
        {
            Serial.print("Fallo, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" intentar de nuevo en 5 segundos");
            delay(5000);
        }
    }
}

void MqttModule::enviarMensajeMQTT(PubSubClient &mqttClient, const String &mensaje, const char* topic)
{
    if (mqttClient.connected())
    {
        mqttClient.publish(topic, mensaje.c_str());
    }
}

void MqttModule::callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Mensaje recibido [");
    Serial.print(topic);
    Serial.print("] ");
    char payload_string[length + 1];
    int resultI;
    memcpy(payload_string, payload, length);
    payload_string[length] = '\0';
    resultI = atoi(payload_string);
    var = resultI;
    resultS = "";
    for (int i = 0; i < length; i++)
    {
        resultS = resultS + (char)payload[i];
    }
    Serial.println(resultS);
    Serial.println();
}