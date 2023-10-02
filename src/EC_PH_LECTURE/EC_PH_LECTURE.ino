#include <Ezo_i2c.h> //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Wire.h>    //include arduinos i2c library
#include <sequencer2.h> //imports a 2 function sequencer 
#include <Ezo_i2c_util.h> //brings in common print statements
#include <WiFi.h>
#include "ph_grav.h"
#include <ThingsBoard.h>

//----------------------------------------------------------------
#include <EEPROM.h>
#include "GravityTDS.h"

#define TdsSensorPin A1
GravityTDS gravityTds;
float temperature = 25,tdsValue = 0;
//----------------------------------------------------------------

//----------------------------------------------------------------
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
#define THINGSBOARD_MQTT_SERVER       "thingsboard.cloud"
#define THINGSBOARD_MQTT_ACESSTOKEN   "9Bvzv2qpM9J6sveLvkLd"
#define THINGSBOARD_MQTT_PORT         1883
//==============================================================================
const char* ssid = "Convergentes";
const char* password = "RedesConvergentes*#";
//==============================================================================
WiFiClient espClient;
ThingsBoard tb(espClient);
int status = WL_IDLE_STATUS;
//==============================================================================
char *server = "10.1.41.223";
int port = 1883;
int var = 0;
char datos[40];
String resultS = "";
float     analog_ph;
//=====================================(Mqtt)=========================================
void callback(char* topic, byte* payload, unsigned int length) {
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
  for (int i=0;i<length;i++) {
    resultS= resultS + (char)payload[i];
  }
  Serial.println();
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Intentando conectarse MQTT...");
    if (mqttClient.connect("arduinoClient")) {
      Serial.println("Conectado");
      mqttClient.subscribe("smartgrow");
    } else {
      Serial.print("Fallo, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" intentar de nuevo en 5 segundos");
      delay(5000);
    }
  }
}
//==============================================================================
String sensorstring = "";
boolean sensor_string_complete = false;

void setup() {
  Wire.begin();                           //start the I2C
  Serial.begin(9600);                     //start the serial communication to the computer
  Seq.reset();                            //initialize the sequencer
//-----------------------------------------------------------------------------
gravityTds.setPin(TdsSensorPin);
    gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
    gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
    gravityTds.begin();  //initialization
//-----------------------------------------------------------------------------
  
  sensorstring.reserve(30);  
  pH.begin();
  //Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Conectando a Wifi");
  while(WiFi.status() != WL_CONNECTED){
    Serial.println(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Wifi Conectado");
  Serial.println(WiFi.localIP());
//==================================(Mqtt)============================================
  mqttClient.setServer(server, port);
  mqttClient.setCallback(callback);
//==============================================================================  
}

void loop() {
  Seq.run();//run the sequncer to do the polling
//==================================(Mqtt)============================================
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
  String json = "{\"EC\":" + String(EC) + ",\"pH\":" + String(ph) + ",\"Sensor\":" + "EC_pH"+"}";
  Serial.println(json);
  mqttClient.publish("smartgrow", json.c_str());
  delay(1000);
//==============================================================================
}

void step1(){
   //send a read command. we use this command instead of PH.send_cmd("R"); 
  //to let the library know to parse the reading
  ph = pH.read_ph();                      
  EC.send_read_cmd();
  analog_ph = float(ph);
  //------------------------------------------------------------------------------
  if(!tb.connected())
    {
    if(!tb.connect(THINGSBOARD_MQTT_SERVER, THINGSBOARD_MQTT_ACESSTOKEN, THINGSBOARD_MQTT_PORT)) 
      {
      Serial.println("Failed to connect");
      return;
      }
    }
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
