#include <Arduino.h> //Required for Visual Studio Code
#include "SparkFun_AS7265X.h"  //Click here to get the library: http://librarymanager/All#SparkFun_AS7265X

AS7265X sensor; //Create instance of the AS7265X sensor

#include <Wire.h> //Include the I2C library
#include <Adafruit_SleepyDog.h> //Include the watchdog library

#define TEL true
#define LOCAL false

#if TEL
#include "WifiModule.h"
#include "MqttModule.h"
#include "HttpModule.h"
#include "SCD40Sensor.h"
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
const char* sensor_id = "650dc7d640e0be7842fc4239"; // ID del sensor SCD40
SCD40Sensor SCD40(sensor_id);

const unsigned long interval = 60000; // Intervalo de tiempo en milisegundos (5 min)
unsigned long previousMillis = 0;
//=======================================================================
void setup()
{
  Serial.begin(115200);
  Wire.begin();
  //=======================================================================
  if (sensor.begin() == false) //Begin returns 1 if device ID is incorrect
  {
    Serial.println("Error: Unable to communicate to AS7265x sensor.");
    while (1)
      ; //Freeze
  }

  #if TEL
  wifiModule.conectarWifi();
  //Watchdog.enable(30000);
  #endif
}
// float ppf(float counts){
//   float A = sensor.getCalibratedA();
//   float B = sensor.getCalibratedB();
//   float C = sensor.getCalibratedC();
//   float D = sensor.getCalibratedD();
//   float E = sensor.getCalibratedE();
//   float F = sensor.getCalibratedF();
//   float G = sensor.getCalibratedG();
//   float H = sensor.getCalibratedH();
//   float I = sensor.getCalibratedI();
//   float J = sensor.getCalibratedJ();
//   float K = sensor.getCalibratedK();
//   float L = sensor.getCalibratedL();
//   float M = sensor.getCalibratedM();
//   float N = sensor.getCalibratedN();
//   float O = sensor.getCalibratedO();
//   float P = sensor.getCalibratedP();
//   float Q = sensor.getCalibratedQ();
//   float R = sensor.getCalibratedR();
// }

void loop()
{
  //=======================================================================
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    SCD40.read();
    #if TEL
    String jsonString = SCD40.buildJson();
    httpClient.enviarDatosHTTP(jsonString.c_str(), "scd40");
    delay(1000);
    #endif
  }
}

