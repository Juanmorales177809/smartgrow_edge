#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <Adafruit_SleepyDog.h>

WiFiMulti wifiMulti;

//Definir red Wifi a conectar el dispositivo
//const char* ssid = "smartgrow";
//const char* password = "2205631700";

const char* ssid = "Convergentes";
const char* password = "RedesConvergentes*#";

//Definicion de Variables
float flujo;
float TempAgua;
float litro;

//Sensor config
#define LED_BUILTIN 2
#define SENSOR  27

long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

int Vo;
float R1 = 51350;
float logR2, R2, TEMPERATURA;
float c1 = 2.099609707e-03, c2 = 0.5081190862e-04, c3 = 5.562986194e-07;
float m = 1.022225554341915, b = 4.054505841335171; 
void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

//Definir lapreparacion del codigo con SETUP
void setup() {
  //Sensor config
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SENSOR, INPUT_PULLUP);

  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;

  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);

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

void loop() {
  Watchdog.reset();
  HTTPClient http;
  Serial.println("[HTTP] Iniciando ... ");
  http.begin("http://172.1.1.19:8000/sensor_de_flujo");
  http.addHeader("Content-Type", "application/json");
  Serial.println("[HTTP] POST...");

  //Wait 1s
  currentMillis = millis();
  if (currentMillis - previousMillis > interval) {

    pulse1Sec = pulseCount;
    pulseCount = 0;

    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();

    flowMilliLitres = (flowRate / 60) * 1000;

    totalMilliLitres += flowMilliLitres;

    flujo = int(flowRate);
    litro = (totalMilliLitres / 1000);

  }

  Vo = analogRead(34);      // lectura de A0
  R2 = R1 * (4095.0 / (float)Vo - 1.0); // conversion de tension a resistencia
  logR2 = log(R2);      // logaritmo de R2 necesario para ecuacion
  TEMPERATURA = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2)); // ecuacion S-H
  TEMPERATURA = TEMPERATURA - 273.15;
  TempAgua = m*TEMPERATURA+b;
  String json = "{\"flujo\":" + String(flujo) + ",\"litros\":" + String(litro) + ",\"temperatura_agua\":" + String(TempAgua) + "}";
  Serial.println(json);
  int httpCode = http.POST(json);
  String payload = http.getString();
  Serial.println(httpCode);
  Serial.println(payload);
  http.end();
  delay(1000); 
}
