#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <Wire.h>
#include "SparkFun_AS7265X.h" //Click here to get the library: http://librarymanager/All#SparkFun_AS7265X
#include "Zanshin_BME680.h"
#include <Adafruit_SleepyDog.h> 

AS7265X sensor;

BME680_Class BME680;

WiFiMulti wifiMulti;


//Definir red Wifi a conectar el dispositivo
//const char* ssid = "smartgrow";
//const char* password = "2205631700";

const char* ssid = "Convergentes";
const char* password = "RedesConvergentes*#";

//Definicion de Variables
float A, B, C, D, E, F, G, H, R, I, S, J, T, U, V, W, K, L;
static int32_t  temp, humidity, pressure, gas;
static float    alt;
String Sensor, TempF, HumidityF, PressureF, GasF;
int8_t temp1, temp2, humidity1, pressure2, gas2;
int16_t humidity2, pressure1, gas1;
float altitude(const int32_t press, const float seaLevel = 1013.25);
float altitude(const int32_t press, const float seaLevel) {
  
  static float Altitude;
  Altitude =
    44330.0 * (1.0 - pow(((float)press / 100.0) / seaLevel, 0.1903));  // Convert into meters
  return (Altitude);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("AS7265x Spectral Triad Example");
 
  sensor.begin();
  sensor.disableIndicator(); //Turn off the blue status LED
  
  BME680.begin(I2C_STANDARD_MODE);
  BME680.setOversampling(TemperatureSensor, Oversample16);  
  BME680.setOversampling(HumiditySensor, Oversample16);     
  BME680.setOversampling(PressureSensor, Oversample16);   
  BME680.setIIRFilter(IIR4);
  BME680.setGas(320, 150);

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

void loop()
{
  Watchdog.reset();
  
  HTTPClient http;
  Serial.println("[HTTP] Iniciando ... ");
  http.begin("http://172.1.1.19:8000/sensor_de_flujo");
  http.addHeader("Content-Type", "application/json");
  Serial.println("[HTTP] POST...");
  
  sensor.takeMeasurementsWithBulb();
  
  A=sensor.getCalibratedA(); // 410 nm
  B=sensor.getCalibratedB(); // 435 nm
  C=sensor.getCalibratedC(); // 460 nm
  D=sensor.getCalibratedD(); // 485 nm
  E=sensor.getCalibratedE(); // 510 nm
  F=sensor.getCalibratedF(); // 535 nm
  G=sensor.getCalibratedG(); // 560 nm
  H=sensor.getCalibratedH(); // 585 nm
  R=sensor.getCalibratedR(); // 610 nm
  I=sensor.getCalibratedI(); // 645 nm
  S=sensor.getCalibratedS(); // 680 nm
  J=sensor.getCalibratedJ(); // 705 nm
  T=sensor.getCalibratedT(); // 730 nm
  U=sensor.getCalibratedU(); // 760 nm
  V=sensor.getCalibratedV(); // 810 nm
  W=sensor.getCalibratedW(); // 860 nm
  K=sensor.getCalibratedK(); // 900 nm
  L=sensor.getCalibratedL(); // 940 nm
  
  BME680.getSensorData(temp, humidity, pressure, gas);
  alt = altitude(pressure);
  temp1 = temp/100;
  temp2 = temp % 100;
  humidity1 = humidity / 1000;
  humidity2 = humidity % 1000;
  pressure1 = pressure / 100;
  pressure2 = pressure % 100;
  gas1 = gas / 100;
  gas2 = gas % 100;
  TempF= String(temp1)+"."+String(temp2);
  HumidityF= String(humidity1)+"."+String(humidity2);
  PressureF= String(pressure1)+"."+String(pressure2);
  GasF= String(gas1)+"."+String(gas2);
  
  Sensor = TempF+"-"+HumidityF+"-"+PressureF+"-"+String(alt)+"-"+GasF ;

  String json = "{\"temperatura\":" + TempF + ",\"humedad\":" + HumidityF + ",\"presion\":" + PressureF + ",\"altitud\":" + String(alt) + ",\"co2\":" + GasF + ",\"410 nm\":" + String(A) + ",\"435 nm\":" + String(B) + ",\"460 nm\":" + String(C) + ",\"480 nm\":" + String(D) + ",\"510 nm\":" + String(E) + ",\"535 nm\":" + String(F) + ",\"560 nm\":" + String(G) + ",\"585 nm\":" + String(H) + ",\"610 nm\":" + String(R) + ",\"645 nm\":" + String(I) + ",\"680 nm\":" + String(S) + ",\"705 nm\":" + String(J) + ",\"730 nm\":" + String(T) + ",\"760 nm\":" + String(U) + ",\"810 nm\":" + String(V) + ",\"860 nm\":" + String(W) + ",\"900 nm\":" + String(K) + ",\"940 nm\":" + String(L) +"}";
  Serial.println(json);
  int httpCode = http.POST(json);
  String payload = http.getString();
  Serial.println(httpCode);
  Serial.println(payload);
  http.end();
  delay(1000); 

}
