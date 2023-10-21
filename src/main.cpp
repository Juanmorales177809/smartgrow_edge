#include <Arduino.h>
#include <Ezo_i2c.h> 
#include <Wire.h>    
#include <sequencer2.h> 
#include <Ezo_i2c_util.h> 
#include "ph_grav.h"
#include <PeristalticsModule.h>

#define TEL true

#if TEL
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFiModule.h"
#include "MqttModule.h"
#include "HttpModule.h"

// WiFi
const char *ssid = ""; // Nombre de la red WiFi
const char *password = ""; // Contraseña de la red WiFi

// MQTT
const char* server = "";
const int mqtt_port = 8310;
const int http_port = 8311;

WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);
String sensorstring = "";
const char* sensor_id = ""; // ID del sensor

const unsigned long interval = 60000; // Intervalo de tiempo en milisegundos (1 min)
unsigned long previousMillis = 0;
#endif

Gravity_pH pH = Gravity_pH(32);
Ezo_board EC = Ezo_board(100, "EC");

void step1();  
void step2();

float TDS_float;
float ph;                 
float temp_float;                
float analog_ph;

Sequencer2 Seq(&step1, 1000, &step2, 0);

void setup()
{
  Wire.begin();
  Serial.begin(115200);
  Seq.reset();
  pH.begin();
  #if TEL
  sensorstring.reserve(30);  
  WiFiModule::conectarWiFi(ssid, password);
  mqttClient.setServer(server, mqtt_port);
  mqttClient.setCallback(MqttModule::callback);  
  #endif
}

void loop()
{
  Seq.run();
  if (!mqttClient.connected()) {
    MqttModule::conectarMQTT(mqttClient, server, mqtt_port);
  }
  #if TEL
  mqttClient.loop();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    StaticJsonDocument<200> jsonDocument; 
    jsonDocument["ph"] = ph;
    jsonDocument["ec"] = EC.get_last_received_reading();
    jsonDocument["temperatura"] = 0.0;
    jsonDocument["sensor"] = sensor_id;

    // Serializar el JSON en una cadena
    String jsonString;
    serializeJson(jsonDocument, jsonString);

    HttpModule::enviarDatosHTTP(server, http_port, jsonString.c_str(), "/phec");
    MqttModule::enviarMensajeMQTT(mqttClient, jsonString);
    delay(1000);
  }
  #endif
}

void step1(){
  EC.send_cmd_with_num("T,", 25.0);
  ph = pH.read_ph();                      
  EC.send_read_cmd();
  analog_ph = float(ph);
  delay(1000);
}

void step2(){
  Serial.print("PH: ");
  Serial.println(ph);
  receive_and_print_reading(EC);             //get the reading from the EC circuit
  
  Serial.println();
  Serial.print("TDS: ");
  Serial.println(EC.get_last_received_reading()*0.5);
}