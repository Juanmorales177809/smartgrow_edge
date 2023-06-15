#include <WiFi.h>
#include <aREST.h>
#include <Adafruit_SleepyDog.h>

// Crear API Rest
aREST rest = aREST();

// Definir red Wifi a conectar el dispositivo
const char* ssid = "Convergentes";
const char* password = "RedesConvergentes*#";

// Definir puertos
#define LISTEN_PORT 80

// Crear Instancia de Servidor
WiFiServer server(LISTEN_PORT);

// Definicion de Variables
float flujo1, flujo2, flujo3;
float tempAgua1, tempAgua2, tempAgua3;
float litro1, litro2, litro3;

// Sensor config
#define LED_BUILTIN 2
#define SENSOR1 27
#define SENSOR2 26
#define SENSOR3 25

long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor1 = 4.5;  // Factor de calibración para sensor de flujo 1
float calibrationFactor2 = 4.2;  // Factor de calibración para sensor de flujo 2
float calibrationFactor3 = 4.8;  // Factor de calibración para sensor de flujo 3
volatile byte pulseCount1, pulseCount2, pulseCount3;
byte pulse1Sec1, pulse1Sec2, pulse1Sec3;
float flowRate1, flowRate2, flowRate3;
unsigned int flowMilliLitres1, flowMilliLitres2, flowMilliLitres3;
unsigned long totalMilliLitres1, totalMilliLitres2, totalMilliLitres3;

int Vo;
float R1 = 51350;
float logR2, R2, TEMPERATURA;
float c1 = 2.099609707e-03, c2 = 0.5081190862e-04, c3 = 5.562986194e-07;

void IRAM_ATTR pulseCounter1() {
  pulseCount1++;
}

void IRAM_ATTR pulseCounter2() {
  pulseCount2++;
}

void IRAM_ATTR pulseCounter3() {
  pulseCount3++;
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

  // Init Variables API
  rest.variable("Flujo1", &flujo1);
  rest.variable("Flujo2", &flujo2);
  rest.variable("Flujo3", &flujo3);
  rest.variable("TemperaturaAgua1", &tempAgua1);
  rest.variable("TemperaturaAgua2", &tempAgua2);
  rest.variable("TemperaturaAgua3", &tempAgua3);
  rest.variable("Litros1", &litro1);
  rest.variable("Litros2", &litro2);
  rest.variable("Litros3", &litro3);

  // Name ID
  rest.set_id("2");
  rest.set_name("Flujo");

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected!");

  // Start Server
  server.begin();
  Serial.println("Server started!");

  // IP
  Serial.println(WiFi.localIP());

  Watchdog.enable(30000);
}

void loop() {
  Watchdog.reset();

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

  Vo = analogRead(34);      // lectura de A0
  R2 = R1 * (4095.0 / (float)Vo - 1.0); // conversion de tension a resistencia
  logR2 = log(R2);      // logaritmo de R2 necesario para ecuacion
  TEMPERATURA = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2)); // ecuacion S-H
  TEMPERATURA = TEMPERATURA - 273.15;
  tempAgua1 = TEMPERATURA;
  
  // Aquí puedes añadir la lectura de los otros sensores de temperatura

  // REST Calls
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  rest.handle(client);
}
