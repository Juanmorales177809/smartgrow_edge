#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "SparkFun_SCD4x_Arduino_Library.h"
#include <Adafruit_SleepyDog.h> 

SCD4x mySensor;

WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);

//Definir red Wifi a conectar el dispositivo
//const char* ssid = "smartgrow";
//const char* password = "2205631700";

//const char* ssid = "Convergentes";
//const char* password = "RedesConvergentes*#";

const char* ssid = "WIFI-ITM";
const char* password = "";

char *server = "10.1.41.223";
int port = 1883;
int var = 0;
char datos[40];
String resultS = "";

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

//Definicion de Variables

float mq, Humidity, temperature;

//Definir lapreparacion del codigo con SETUP
void setup(){
  Serial.begin(115200);   
  Wire.begin();          
  mySensor.begin();
  
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
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());
  mqttClient.setServer(server, port);
  mqttClient.setCallback(callback);  
  
  Watchdog.enable(30000);

}

//Ciclo Loop para sensar constantemente
void loop(){
  Watchdog.reset();
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
  mySensor.readMeasurement();
//Lectura de Sensores
  mq = mySensor.getCO2();    //MQ 135 Concentracion de Gases
  Humidity = mySensor.getHumidity(); // Humedad Relativa 0% - 100% (Punto de Rocio)
  temperature = mySensor.getTemperature();  // Temperatura 0°C - 120°C
  String json = "{\"co2\":" + String(mq) + ",\"humedad\":" + String(Humidity) + ",\"temperatura\":" + String(temperature) + ",\"Sensor\":" + "SCD40"+"}";
  Serial.println(json);
  mqttClient.publish("smartgrow", json.c_str());
  delay(1000);
  }
