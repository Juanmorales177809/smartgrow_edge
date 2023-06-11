#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <Wire.h>
#include "SparkFun_SCD4x_Arduino_Library.h"
#include <Adafruit_SleepyDog.h> 

SCD4x mySensor;

WiFiMulti wifiMulti;

//Definir red Wifi a conectar el dispositivo
//const char* ssid = "smartgrow";
//const char* password = "2205631700";

//const char* ssid = "Convergentes";
//const char* password = "RedesConvergentes*#";

const char* ssid = "WIFI-ITM";
const char* password = "";

//Definicion de Variables

float mq, Humidity, temperature;


//Definir lapreparacion del codigo con SETUP
void setup(){
  Serial.begin(115200);   
  Wire.begin();          
  mySensor.begin();

  //The SCD4x has data ready every five seconds

  //Connect to WiFi
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(ssid, password);
  Serial.println("Conectando a Wifi");
  while(wifiMulti.run() != WL_CONNECTED){
    Serial.println(".");
  }
  Serial.println();
  Serial.println("Wifi Conectado");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());
  
  
  Watchdog.enable(30000);

}


//Ciclo Loop para sensar constantemente
void loop(){
  Watchdog.reset();
  HTTPClient http;
  Serial.println("[HTTP] Iniciando ... ");
  //http.begin("http://172.1.1.19:8000/scd40");
  http.begin("http://10.1.36.74:8000/scd40");
  http.addHeader("Content-Type", "application/json");
  Serial.println("[HTTP] POST...");
  
  mySensor.readMeasurement();
//Lectura de Sensores
  mq = mySensor.getCO2();    //MQ 135 Concentracion de Gases
  Humidity = mySensor.getHumidity(); // Humedad Relativa 0% - 100% (Punto de Rocio)
  temperature = mySensor.getTemperature();  // Temperatura 0°C - 120°C
  String json = "{\"co2\":" + String(mq) + ",\"humedad\":" + String(Humidity) + ",\"temperatura\":" + String(temperature) + "}";
  Serial.println(json);
  int httpCode = http.POST(json);
  String payload = http.getString();
  Serial.println(httpCode);
  Serial.println(payload);
  http.end();
  delay(1000); 
  
  }
