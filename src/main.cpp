#include <Arduino.h> //Required for Visual Studio Code
#include "SCD40Sensor.h"
#include "AS7265xModule.h"

#include <Wire.h> //Include the I2C library
#include <Adafruit_SleepyDog.h> //Include the watchdog library

#define TEL true  // true para enviar datos a servidor, false para no enviar datos
#define LOCAL true // true para servidor local, false para servidor remoto
#define SENSORID1 true // true para sensor 1, false para sensor 2
#define SCD4 false // true para sensor SCD40
#define AS72 true // true para sensor AS7265X

#if TEL
#include "WifiModule.h"
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
#else
const char *ssid = "Familia Morales"; // Nombre de la red WiFi
const char *password = "2205631700"; // Contraseña de la red WiFi
const char* server = "200.122.207.134"; // IP publica del servidor MQTT
const int http_port = 8311;
const int mqtt_port = 8310;
#endif
WifiModule wifiModule(ssid, password);
HttpModule httpClient(server, http_port);

#endif
//=======================================================================
#if SENSORID1
const char* sensor_id = "650dc7d640e0be7842fc4239"; // ID del sensor SCD40_1
#else
const char* sensor_id = "65391fa4700d51b6d681b3c5"; // ID del sensor SCD40_2
#endif
const char* sensor_id2 = "655dd4a264d0cd6c1628e4b3"; // ID del sensor AS7265X
#if SCD4
SCD40Sensor SCD40(sensor_id);
#endif
#if AS72
AS7265xModule AS7265X(sensor_id);
#endif
//=======================================================================
const unsigned long interval = 300000; // Intervalo de tiempo en milisegundos (5 min)
unsigned long previousMillis = 0;
//=======================================================================
void setup()
{
  Watchdog.enable(360000);
  Serial.begin(115200);
  #if SCD4
  Wire.begin();
  SCD40.begin(Wire);
  #endif
  #if AS72
  Serial.println("AS7265x Example");
  AS7265X.begin("1X", 49);
  #endif
  //=======================================================================
   #if TEL
  wifiModule.conectarWifi();
  #endif
}
void loop()
{
  //=======================================================================

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    #if SCD4
    SCD40.read();
    #endif
    #if AS72
    AS7265X.Read();
    #endif
    #if TEL
    #if SCD4
    String jsonString = SCD40.buildJson();
    httpClient.enviarDatosHTTP(jsonString.c_str(), "scd40");
    #endif
    #if AS72
    String jsonString2 = AS7265X.buildJson();
    httpClient.enviarDatosHTTP(jsonString2.c_str(), "as7265x");
    #endif
    delay(1000);
    #endif
    Watchdog.reset();
  }
}

