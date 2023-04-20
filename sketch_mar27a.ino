#include <WiFi.h>
#include<aREST.h>
#include <Wire.h>
#include "SparkFun_SCD4x_Arduino_Library.h" 
//#include "Zanshin_BME680.h"

SCD4x mySensor;

//BME680_Class BME680;


//Crear API Rest
aREST rest = aREST();


//Definir red Wifi a conectar el dispositivo
const char* ssid = "smartgrow";
const char* password = "2205631700";
//const char* ssid = "Convergentes";
//const char* password = "RedesConvergentes*#";

// Definir puertos
#define LISTEN_PORT 80

//Crear Instancia de Servidor
WiFiServer server(LISTEN_PORT);


//Definicion de Variables
//String sen1, sen2;
float mq, Humidity, temperature;
//float altitude(const int32_t press, const float seaLevel = 1013.25);
//float altitude(const int32_t press, const float seaLevel) {
//  /*!
//  @brief     
//  @details   
//  @param[in] press    
//  @param[in] seaLevel 
//  @return    
//  */
//  static float Altitude;
//  Altitude =
//      44330.0 * (1.0 - pow(((float)press / 100.0) / seaLevel, 0.1903));  // Convert into meters
//  return (Altitude);
//} 

unsigned long previousMillis = 0;
unsigned long interval = 30000;  // Intervalo de tiempo en milisegundos para el watchdog

//Definir lapreparacion del codigo con SETUP
void setup(){
  Serial.begin(115200);   // para que lo ssensores lean a la misma velocidad
  Wire.begin();          // Inicializar el sensor
  mySensor.begin();
//Inicializar Variables API
  rest.variable("temperature",&temperature);
  rest.variable("humidity",&Humidity);
  rest.variable("contaminacion",&mq);
//  rest.variable("temperature2",&temp);
//  rest.variable("humidity2",&humidity);
//  rest.variable("pressure",&pressure);
//  rest.variable("gas",&gas);
//  rest.variable("alt",&alt);
//  rest.variable("SCD40",&sen1);
//  rest.variable("BME680",&sen2);
// Nombres de los Dispositivos
  rest.set_id("1");
  rest.set_name("sensor");

  //The SCD4x has data ready every five seconds

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //Cliclo While para verificar coneccion a wifi. tiempo en milisegundos
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
  Serial.println("");
  Serial.println("WiFi connected!");

// Inicializar Servidor
  server.begin();
  Serial.println("Server Started!");

// IP
  Serial.println(WiFi.localIP());
  
//  BME680.setOversampling(TemperatureSensor, Oversample16);  // Use enumerated type values
//  BME680.setOversampling(HumiditySensor, Oversample16);     // Use enumerated type values
//  BME680.setOversampling(PressureSensor, Oversample16);     // Use enumerated type values
//  BME680.setIIRFilter(IIR4);
//  BME680.setGas(320, 150);
}


//Ciclo Loop para sensar constantemente
void loop(){
  
  
  mySensor.readMeasurement();
//Lectura de Sensores
  mq = mySensor.getCO2();    //MQ 135 Concentracion de Gases
  Humidity = mySensor.getHumidity(); // Humedad Relativa 0% - 100% (Punto de Rocio)
  temperature = mySensor.getTemperature();  // Temperatura 0°C - 120°C
  
//  static int32_t  temp, humidity, pressure, gas;
//  static float    alt;
//  BME680.getSensorData(temp, humidity, pressure, gas);
//  sen1 = String(mq)+"-"+String(Humidity)+"-"+String(temperature);
//  sen2 = String(temp)+"-"+String(humidity)+"-"+String(pressure)+"-"+String(gas)+"-"+String(alt);

 // Llamados al API Rest
   WiFiClient client = server.available(); // esta disponible el servidor
  if (!client){
    return;
    }
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis=currentMillis;
    ESP.restart();
  }
//  while(!client.available()){
//    delay(1);
//    }
  rest.handle(client);
  }
