#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_SleepyDog.h>

WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);

// Definir red Wifi a conectar el dispositivo
const char* ssid = "smartgrow";
const char* password = "2205631700";

// Definicion de Variables
float flujo1, flujo2, flujo3;
float tempAgua1, tempAgua2;
float litro1, litro2, litro3;

// Sensor config
#define LED_BUILTIN 2
#define SENSOR1 33
#define SENSOR2 27
#define SENSOR3 12

long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor1 = 4.5;  // Factor de calibración para sensor de flujo 1
float calibrationFactor2 = 4.5;  // Factor de calibración para sensor de flujo 2
float calibrationFactor3 = 4.5;  // Factor de calibración para sensor de flujo 3
volatile byte pulseCount1, pulseCount2, pulseCount3;
byte pulse1Sec1 = 0;
byte pulse1Sec2 = 0;
byte pulse1Sec3 = 0;
float flowRate1, flowRate2, flowRate3;
unsigned int flowMilliLitres1, flowMilliLitres2, flowMilliLitres3;
unsigned long totalMilliLitres1, totalMilliLitres2, totalMilliLitres3;

int Vo1,Vo2;
float R11 = 51350;
float R12 = 51350;
float logR21, R21, TEMPERATURA1,logR22, R22, TEMPERATURA2;
float c1 = 2.099609707e-03, c2 = 0.5081190862e-04, c3 = 5.562986194e-07;
float m = 1.022225554341915, b = 4.054505841335171;

void IRAM_ATTR pulseCounter1() {
  pulseCount1++;
}

void IRAM_ATTR pulseCounter2() {
  pulseCount2++;
}

void IRAM_ATTR pulseCounter3() {
  pulseCount3++;
}

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

// Definir la preparación del código con SETUP
void setup() {
  // Sensor config
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SENSOR1, INPUT_PULLUP);
  pinMode(SENSOR2, INPUT_PULLUP);
  pinMode(SENSOR3, INPUT_PULLUP);

  pulseCount1 = 0;
  pulseCount2 = 0;
  pulseCount3 = 0;
  flowRate1 = 0.0;
  flowRate2 = 0.0;
  flowRate3 = 0.0;
  flowMilliLitres1 = 0;
  flowMilliLitres2 = 0;
  flowMilliLitres3 = 0;
  totalMilliLitres1 = 0;
  totalMilliLitres2 = 0;
  totalMilliLitres3 = 0;
  previousMillis = 0;

  attachInterrupt(digitalPinToInterrupt(SENSOR1), pulseCounter1, FALLING);
  attachInterrupt(digitalPinToInterrupt(SENSOR2), pulseCounter2, FALLING);
  attachInterrupt(digitalPinToInterrupt(SENSOR3), pulseCounter3, FALLING);

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

void loop() {
  Watchdog.reset();
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
  // Wait 1s
  currentMillis = millis();
  if (currentMillis - previousMillis > interval) {

    pulse1Sec1 = pulseCount1;
    pulse1Sec2 = pulseCount2;
    pulse1Sec3 = pulseCount3;
    pulseCount1 = 0;
    pulseCount2 = 0;
    pulseCount3 = 0;

    flowRate1 = ((1000.0 / (millis() - previousMillis)) * pulse1Sec1) / calibrationFactor1;
    flowRate2 = ((1000.0 / (millis() - previousMillis)) * pulse1Sec2) / calibrationFactor2;
    flowRate3 = ((1000.0 / (millis() - previousMillis)) * pulse1Sec3) / calibrationFactor3;
    previousMillis = millis();

    flowMilliLitres1 = (flowRate1 / 60) * 1000;
    flowMilliLitres2 = (flowRate2 / 60) * 1000;
    flowMilliLitres3 = (flowRate3 / 60) * 1000;

    totalMilliLitres1 += flowMilliLitres1;
    totalMilliLitres2 += flowMilliLitres2;
    totalMilliLitres3 += flowMilliLitres3;

    flujo1 = int(flowRate1);
    flujo2 = int(flowRate2);
    flujo3 = int(flowRate3);
    litro1 = (totalMilliLitres1 / 1000);
    litro2 = (totalMilliLitres2 / 1000);
    litro3 = (totalMilliLitres3 / 1000);
  }

  Vo1 = analogRead(34);     // lectura de A0
  Vo2 = analogRead(25);
  
  R21 = R11 * (4095.0 / (float)Vo1 - 1.0); // conversion de tension a resistencia
  logR21 = log(R21);      // logaritmo de R2 necesario para ecuacion
  TEMPERATURA1 = (1.0 / (c1 + c2 * logR21 + c3 * logR21 * logR21 * logR21)); // ecuacion S-H
  R22 = R12 * (4095.0 / (float)Vo2 - 1.0);
  logR22 = log(R22);
  TEMPERATURA2 = (1.0 / (c1 + c2 * logR22 + c3 * logR22 * logR22 * logR22));
  R23 = R13 * (4095.0 / (float)Vo3 - 1.0);
  logR23 = log(R23);
  
  tempAgua1 = m*TEMPERATURA1+b;
  TEMPERATURA2 = TEMPERATURA2 - 273.15;
  tempAgua2 = m*TEMPERATURA2+b;


  String json = "{\"litros_1\":" + String(litros1) + ",\"temperatura_agua1\":" + String(tempAgua1) + ",\"litros_2\":" + String(litros2) + ",\"temperatura_agua2\":" + String(tempAgua2) + ",\"litros_3\":" + String(litros3) + ",\"Sensor\":" + "hidroponico" +"}";
  Serial.println(json);
  mqttClient.publish("smartgrow", json.c_str());
  delay(1000); 

}
