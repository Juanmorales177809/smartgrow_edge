#include <WiFi.h>
#include <PubSubClient.h>
#include "ph_grav.h"
//#include "ph_iso_grav.h"       
Gravity_pH_Isolated pH = Gravity_pH_Isolated(A0);

WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);

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

String sensorstring = "";
boolean sensor_string_complete = false;
float ph;

void setup(){
  Serial.begin(115200); 
  Serial3.begin(115200);
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
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());
  mqttClient.setServer(server, port);
  mqttClient.setCallback(callback);  
  
  Watchdog.enable(30000);

}

void serialEvent3() {                                 
  sensorstring = Serial3.readStringUntil(13);         
  sensor_string_complete = true;                      
}

void loop(){
  Watchdog.reset();
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
  
  ph = pH.read_ph();

  if (sensor_string_complete == true) {               
    if (isdigit(sensorstring[0]) == false) {          
      Serial.println(sensorstring);                   
    }
    else {
      char sensorstring_array[30];
      char *EC;                                            
      sensorstring.toCharArray(sensorstring_array, 30);
      EC = strtok(sensorstring_array, ",");                                
    }
    sensorstring = "";                                
    sensor_string_complete = false;                   
  }
}
  
  String json = "{\"EC\":" + String(EC) + ",\"pH\":" + String(ph) + ",\"Sensor\":" + "EC_pH"+"}";
  Serial.println(json);
  mqttClient.publish("smartgrow", json.c_str());
  delay(1000);
  }
