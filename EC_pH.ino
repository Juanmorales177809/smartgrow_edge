#include <WiFi.h>
#include <PubSubClient.h>
//#include "ph_grav.h"
#include "ph_grav.h"
#include "AtlasSerialSensor.h"
#include <Adafruit_SleepyDog.h>

Gravity_pH pH = Gravity_pH(26);

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3c 
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET -1  
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);

const char* ssid = "Convergentes";
const char* password = "RedesConvergentes*#";

char *server = "172.1.1.253";
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

#define MQTTON 34
#define MQTTOFF 35

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Intentando conectarse MQTT...");
    if (mqttClient.connect("arduinoClient")) {
      Serial.println("Conectado");
      mqttClient.subscribe("smartgrow");
      digitalWrite(MQTTOFF, LOW);
      digitalWrite(MQTTON, HIGH);
    } else {
      Serial.print("Fallo, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" intentar de nuevo en 5 segundos");
      digitalWrite(MQTTOFF, HIGH);
      digitalWrite(MQTTON, LOW);
      delay(5000);
    }
  }
}

float ph, EC;

#define EC_RX 12
#define EC_TX 27
AtlasSerialSensor ecSensor = AtlasSerialSensor(EC_RX, EC_TX);


void setup(){
  Serial.begin(115200);
  display.begin(i2c_Address, true);
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(2);
  display.setTextWrap(false);
  
  pinMode(MQTTON, OUTPUT);
  pinMode(MQTTOFF, OUTPUT);
  digitalWrite(MQTTOFF, HIGH);
  digitalWrite(MQTTON, LOW);
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
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());
  mqttClient.setServer(server, port);
  mqttClient.setCallback(callback);  
  
  Watchdog.enable(30000);

}



void loop(){
  Watchdog.reset();
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
  
  ph = pH.read_ph();
  EC = ecSensor.getReading();
  
  String json = "{\"EC\":" + String(EC) + ",\"pH\":" + String(ph) + ",\"Sensor\":" + "EC_pH"+"}";
  Serial.println(json);
  mqttClient.publish("smartgrow", json.c_str());
    display.setCursor(0, 10);
  display.print("pH: ");
  display.print(String(EC));
  display.setCursor(0, 30);
  display.print("EC: ");
  display.print(String(ph));
//  display.setCursor(0, 50);
//  display.print("T: ");
//  display.print(value3);
  display.display();
  delay(1000);
  }
