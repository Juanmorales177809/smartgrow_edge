#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include "SparkFun_SCD4x_Arduino_Library.h"
#include <Adafruit_SleepyDog.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFiModule.h"
#include "MqttModule.h"
#include "HttpModule.h"

// WiFi
const char *ssid = ""; // Nombre de la red WiFi
const char *password = ""; // Contraseña de la red WiFi

// MQTT
const char* server = "200.122.207.134";
const int mqtt_port = 8310;
const int http_port = 8311;

WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);
SCD4x mySensor;

float co2,temperature,humedad;
const char* sensor_id = "650dc7d640e0be7842fc4239"; // ID del sensor

const unsigned long interval = 60000; // Intervalo de tiempo en milisegundos (1 min)
unsigned long previousMillis = 0;

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  mySensor.begin();
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
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    mySensor.readMeasurement();
    StaticJsonDocument<200> jsonDocument; // Ajusta el tamaño según tus necesidades
    jsonDocument["co2"] = mySensor.getCO2();
    jsonDocument["temperatura"] = mySensor.getTemperature();
    jsonDocument["humedad"] = mySensor.getHumidity();
    jsonDocument["sensor"] = sensor_id;

    // Serializar el JSON en una cadena
    String jsonString;
    serializeJson(jsonDocument, jsonString);

    // Enviar datos
    HttpModule::enviarDatosHTTP(server, http_port, jsonString.c_str());
    String topic = "smartgrow/sensores/scd40";
    MqttModule::enviarMensajeMQTT(mqttClient, jsonString, topic);
    delay(1000);
    delay(1000);
  }
}