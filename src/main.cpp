#include <Arduino.h>
#include "SparkFun_SCD4x_Arduino_Library.h"
#include <Adafruit_SleepyDog.h>

#define ID1 false
#define LOCAL false
#define TEL true
#if TEL
#include <WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFiModule.h"
#include "MqttModule.h"
#include "HttpModule.h"
#endif

//=======================================================================
#if TEL
#if LOCAL
const char *ssid = "Convergentes"; // Nombre de la red WiFi
const char *password = "RedesConvergentes*#"; // Contraseña de la red WiFi
const char* server = "172.16.20.94"; // IP local del servidor MQTT
const int mqtt_port = 1883;
const int http_port = 3000;
#endif
const char *ssid = "Familia Morales"; // Nombre de la red WiFi
const char *password = "2205631700"; // Contraseña de la red WiFi
const char* server = "200.122.207.134"; // IP publica del servidor MQTT
const int mqtt_port = 8310;
const int http_port = 8311;


WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);

#endif
//=======================================================================
SCD4x mySensor;
float co2,temperatura,humedad, VPD;
#if ID1
const char* sensor_id = "650dc7d640e0be7842fc4239"; // ID del sensor 1
#endif
const char* sensor_id = "65391fa4700d51b6d681b3c5"; // ID del sensor 2
const unsigned long interval = 1000; // Intervalo de tiempo en milisegundos (5 min)
unsigned long previousMillis = 0;


void setup()
{
  Serial.begin(115200);
  Wire.begin();
  mySensor.begin();
  #if TEL
  WiFiModule::conectarWiFi(ssid, password);
  mqttClient.setServer(server, mqtt_port);
  mqttClient.setCallback(MqttModule::callback);
  Watchdog.enable(30000);
}
#endif
float calcularVPD(float temperatura, float humedad){
  
  VPD = 0.611 * exp((17.27 * temperatura) / (temperatura + 237.3)) - (humedad / 100) * 0.611 * exp((17.27 * temperatura) / (temperatura + 237.3));//Calculo de VPD  con la formula de Buck 1996 (https://es.wikipedia.org/wiki/Presi%C3%B3n_de_vapor_de_agua)
  return VPD; 
}
void loop()
{
  #if TEL
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
    temperatura = mySensor.getTemperature();
    humedad = mySensor.getHumidity();
    co2 = mySensor.getCO2();
    VPD = calcularVPD(temperatura, humedad);
    StaticJsonDocument<200> jsonDocument; // Ajusta el tamaño según tus necesidades
    jsonDocument["co2"] = mySensor.getCO2();
    jsonDocument["temperatura"] = mySensor.getTemperature();
    jsonDocument["humedad"] = mySensor.getHumidity();
    jsonDocument["VPD"] = VPD;
    jsonDocument["sensor"] = sensor_id;
    Serial.println("Datos: " + String(mySensor.getCO2()) + " " + String(mySensor.getTemperature()) + " " + String(mySensor.getHumidity()) + " " + String(VPD) + " " + String(sensor_id));
    // Serializar el JSON en una cadena
    String jsonString;
    serializeJson(jsonDocument, jsonString);

    // Enviar datos
    HttpModule::enviarDatosHTTP(server, http_port, jsonString.c_str());
    String topic = "smartgrow/sensores/scd40";
    MqttModule::enviarMensajeMQTT(mqttClient, jsonString, topic);
    delay(1000);
    #endif
    Serial.println("Datos: " + String(mySensor.getCO2()) + " " + String(mySensor.getTemperature()) + " " + String(mySensor.getHumidity()) + " " + String(VPD) + " " + String(sensor_id));
  }
}

