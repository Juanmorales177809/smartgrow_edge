#include <Arduino.h>
#include <Adafruit_SleepyDog.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "WiFiModule.h"
#include "MqttModule.h"
#include "ActuadoresModule.h"

#define LOCAL false
#define RASPBERRY false
// WiFi
#if LOCAL
const char *ssid = "Convergentes";            // Nombre de la red WiFi
const char *password = "RedesConvergentes*#"; // Contraseña de la red WiFi
#else
const char *ssid = "Familia Morales"; // Nombre de la red WiFi
const char *password = "2205631700";  // Contraseña de la red WiFi
#if RASPBERRY
const char *server = "192.168.1.112";
const int mqtt_port = 1883;
#else
const char *server = "200.122.207.134";
const int mqtt_port = 8310;
#endif
#endif
// MQTT

WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);

int var = 0;
String resultS = "";

void acciones(String mensaje)
{
  if (mensaje == "entrada_de_agua_hidroponico_prueba")
  {
    ActuadoresModule::ToggleActuador(ENTRADA_HIDROPONICO, "65999eeb0a40e026b2620ffa");
  }
  else if (mensaje == "desague_hidroponico_prueba")
  {
    if (digitalRead(RECIRCULACION_HIDROPONICO) == HIGH)
    {
      ActuadoresModule::ToggleActuador(DESAGUE_HIDROPONICO, "65999ebf0a40e026b2620ff9");
      ActuadoresModule::ToggleActuador(MOTO_BOMBA, "");
    }
    else
    {
      Serial.println("No se puede desaguar si esta activa la recirculacion");
    }
  }
  else if (mensaje == "recirculacion_hidroponico_prueba")
  {
    if (digitalRead(DESAGUE_HIDROPONICO) == LOW)
    {
      ActuadoresModule::ToggleActuador(DESAGUE_HIDROPONICO, "65999ebf0a40e026b2620ff9");
      ActuadoresModule::ToggleActuador(MOTO_BOMBA, "");
      delay(1000);
    }
    ActuadoresModule::ToggleActuador(RECIRCULACION_HIDROPONICO, "65999fdb0a40e026b2621002");
    ActuadoresModule::ToggleActuador(MOTO_BOMBA, "");
  }
  else
  {
    Serial.println("Mensaje no reconocido");
  }
  StaticJsonDocument<200> jsonDocument;
  jsonDocument["entrada_de_agua"] = digitalRead(ENTRADA_HIDROPONICO);
  jsonDocument["salida_de_agua"] = digitalRead(DESAGUE_HIDROPONICO);
  jsonDocument["recirculacion"] = digitalRead(RECIRCULACION_HIDROPONICO);
  String jsonString;
  serializeJson(jsonDocument, jsonString);
  MqttModule::enviarMensajeMQTT(mqttClient, jsonString, "smartgrow/hidroponico/actuadores/estado");
  String msj = "Entrada de agua: " + String(digitalRead(ENTRADA_HIDROPONICO)) + "\n" +
               "Desague: " + String(digitalRead(DESAGUE_HIDROPONICO)) + "\n" +
               "Recirculacion: " + String(digitalRead(RECIRCULACION_HIDROPONICO)) + "\n" +
               "Moto bomba: " + String(digitalRead(MOTO_BOMBA)) + "\n";
  Serial.println(msj);
}

void callback(char *topic, byte *payload, unsigned int length)
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
  acciones(resultS);
}

void setup()
{
  Serial.begin(115200);
  pinMode(ENTRADA_HIDROPONICO, OUTPUT);
  pinMode(DESAGUE_HIDROPONICO, OUTPUT);
  pinMode(RECIRCULACION_HIDROPONICO, OUTPUT);
  pinMode(MOTO_BOMBA, OUTPUT);
  pinMode(LED_CONNECTION, OUTPUT);
  pinMode(LED_NOT_CONNECTION, OUTPUT);
  WiFiModule::conectarWiFi(ssid, password);
  ActuadoresModule::configInit();
  mqttClient.setServer(server, mqtt_port);
  mqttClient.setCallback(callback);
  Watchdog.enable(30000);
}

void loop()
{
  Watchdog.reset();
  if (!mqttClient.connected())
  {
    MqttModule::conectarMQTT(mqttClient, server, mqtt_port);
  }
  mqttClient.loop();
}
