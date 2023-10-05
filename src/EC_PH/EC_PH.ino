#include <Arduino.h>
#include <Ezo_i2c.h> //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Wire.h>    //include arduinos i2c library
#include <sequencer2.h> //imports a 2 function sequencer 
#include <Ezo_i2c_util.h> //brings in common print statements
#include <WiFi.h>
#include "ph_grav.h"
//#include <ThingsBoard.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "WifiModule.h"
#include "MqttModule.h"

Gravity_pH pH = Gravity_pH(32);
Ezo_board EC = Ezo_board(100, "EC");      //create an EC circuit object who's address is 100 and name is "EC"
//----------------------------------------------------------------
void step1();  //forward declarations of functions to use them in the sequencer before defining them
void step2();
//----------------------------------------------------------------

float TDS_float;                 //float var used to hold the float value of the total dissolved solids.
float ph;                        //float var used to hold the float value of the specific PH.
//----------------------------------------------------------------
Sequencer2 Seq(&step1, 1000, &step2, 0);  //calls the steps in sequence with time in between them
//==============================================================================
//#define THINGSBOARD_MQTT_SERVER       "thingsboard.cloud"
//#define THINGSBOARD_MQTT_ACESSTOKEN   "9Bvzv2qpM9J6sveLvkLd"
//#define THINGSBOARD_MQTT_PORT         1883
//==============================================================================
const char* ssid = "Convergentes";
const char* password = "RedesConvergentes*#";
//==============================================================================
WiFiClient espClient;
PubSubClient mqttClient(espClient);
//ThingsBoard tb(espClient);
int status = WL_IDLE_STATUS;
//==============================================================================
// MQTT
// const char* server = "200.122.207.134";
// const int mqtt_port = 8310;
// const int http_port = 8311;
const char* server = "172.16.20.94";
const int mqtt_port = 1883;
const int http_port = 3000;

float     analog_ph;
//==============================================================================
String sensorstring = "";
boolean sensor_string_complete = false;

const char* sensor_id = "651b3c1a60ccd1c529a301d5"; // ID del sensor
const unsigned long interval = 60000; // Intervalo de tiempo en milisegundos (10 min)
unsigned long previousMillis = 0;

void setup() {
  Wire.begin();                           //start the I2C
  Serial.begin(9600);                     //start the serial communication to the computer
  Seq.reset();                            //initialize the sequencer
//-----------------------------------------------------------------------------
// gravityTds.setPin(TdsSensorPin);
//     gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
//     gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
//     gravityTds.begin();  //initialization
//-----------------------------------------------------------------------------
  
  sensorstring.reserve(30);  
  pH.begin();
  WiFiModule::conectarWiFi(ssid, password);
  mqttClient.setServer(server, mqtt_port);
  mqttClient.setCallback(MqttModule::callback);  
}

void loop() {
  Seq.run();                              //run the sequncer to do the polling
    //if (!mqttClient.connected()) {
    //MqttModule::conectarMQTT(mqttClient, server, mqtt_port);
  //}
  mqttClient.loop();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    StaticJsonDocument<200> jsonDocument; // Ajusta el tamaño según tus necesidades
    jsonDocument["ph"] = ph;
    jsonDocument["ec"] = EC.get_last_received_reading();
    jsonDocument["temperatura"] = 0.0;
    jsonDocument["sensor"] = sensor_id;

    // Serializar el JSON en una cadena
    String jsonString;
    serializeJson(jsonDocument, jsonString);

    // Enviar datos
    HTTPClient http;
    Serial.println("[HTTP] Iniciando ... ");
    http.begin("http://" + String(server) + ":" + String(http_port) + "/phec");
    http.addHeader("Content-Type", "application/json");
    Serial.println("[HTTP] POST...");
    Serial.println(jsonString);
    int httpCode = http.POST(jsonString);
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    http.end();
    //HttpModule::enviarDatosHTTP(server, http_port, jsonString.c_str());
    String topic = "smartgrow/sensores/ph_ec";
    MqttModule::enviarMensajeMQTT(mqttClient, jsonString, topic);
    delay(1000);
  }
}

void step1(){
   //send a read command. we use this command instead of PH.send_cmd("R"); 
  //to let the library know to parse the reading
  EC.send_cmd_with_num("T,", 25.0);
  ph = pH.read_ph();                      
  EC.send_read_cmd();
  analog_ph = float(ph);
  //------------------------------------------------------------------------------
  
//================================================================================
}

void step2(){
  Serial.print("PH: ");
  Serial.println(ph);
  receive_and_print_reading(EC);             //get the reading from the EC circuit
  
  Serial.println();
  Serial.print("TDS: ");
  Serial.println(EC.get_last_received_reading()*0.5);
}