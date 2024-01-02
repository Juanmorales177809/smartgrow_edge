#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_SleepyDog.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFiModule.h"
#include "MqttModule.h"
#include "ActuadoresModule.h"


#define LOCAL false
#define RASPBERRY false
// WiFi
#if LOCAL
const char *ssid = "Convergentes"; // Nombre de la red WiFi
const char *password = "RedesConvergentes*#"; // Contraseña de la red WiFi
#else
const char *ssid = "Familia Morales"; // Nombre de la red WiFi
const char *password = "2205631700"; // Contraseña de la red WiFi
#if RASPBERRY
const char *server = "192.168.1.112";
const int mqtt_port = 1883;
#else
const char* server = "200.122.207.134";
const int mqtt_port = 8310;
#endif
#endif
// MQTT


WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);

void setup()
{
  Serial.begin(115200);
  pinMode(ENTRADA_HIDROPONICO, OUTPUT);
  pinMode(DESAGUE_HIDROPONICO, OUTPUT);
  pinMode(RECIRCULACION_HIDROPONICO, OUTPUT);
  pinMode(MOTO_BOMBA, OUTPUT);
  pinMode(LED_CONNECTION, OUTPUT);
  pinMode(LED_NOT_CONNECTION, OUTPUT);
  ActuadoresModule::configInit();
  WiFiModule::conectarWiFi(ssid, password);
  mqttClient.setServer(server, mqtt_port);
  mqttClient.setCallback(MqttModule::callback);
  Watchdog.enable(30000);
}

void loop()
{
  Watchdog.reset();
  if (!mqttClient.connected()) {
    MqttModule::conectarMQTT(mqttClient, server, mqtt_port);
  }
  mqttClient.loop();
}