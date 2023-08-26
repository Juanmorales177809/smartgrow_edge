#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include "SparkFun_SCD4x_Arduino_Library.h"
#include <Adafruit_SleepyDog.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

SCD4x mySensor;

WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);

const char *ssid = "WIFI-ITM";
const char *password = "";

char *server = "10.1.39.153";
int port = 1883;
int var = 0;
char datos[40];
String resultS = "";

float mq, Humidity, temperature;

const unsigned long interval = 600000; // Intervalo de 10 minutos en milisegundos
unsigned long previousMillis = 0;

void accionesMQTT(String mensaje)
{
  if (mensaje == "recirculacion")
  {
    Serial.println("Recirculacion");
  }
  else if (mensaje == "hidroponico")
  {
    Serial.println("Recirculacion");
  }
}

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
  accionesMQTT(resultS);
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

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  mySensor.begin();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Conectando a Wifi");
  while (WiFi.status() != WL_CONNECTED)
  {
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

void loop()
{
  Watchdog.reset();
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    mySensor.readMeasurement();
    StaticJsonDocument<200> jsonDocument; // Ajusta el tamaño según tus necesidades
    jsonDocument["co2"] = mySensor.getCO2();
    jsonDocument["temperatura"] = mySensor.getTemperature();
    jsonDocument["humedad"] = mySensor.getHumidity();
    jsonDocument["tipo"] = "hidro";

    // Serializar el JSON en una cadena
    String jsonString;
    serializeJson(jsonDocument, jsonString);

    HTTPClient http;
    Serial.println("[HTTP] Iniciando ... ");
    http.begin("http://10.1.39.153:3000/scd40");
    http.addHeader("Content-Type", "application/json");
    Serial.println("[HTTP] POST...");
    Serial.println(jsonString);
    int httpCode = http.POST(jsonString);
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    http.end();
    mqttClient.publish("smartgrow", jsonString.c_str());
    delay(1000);
  }
}