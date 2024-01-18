#include <Arduino.h> //Required for Visual Studio Code
#include <Wire.h> //Include the I2C library
#include <Adafruit_SleepyDog.h> //Include the watchdog library

#define TEL true  // true para enviar datos a servidor, false para no enviar datos
#define LOCAL false // true para servidor local, false para servidor remoto
#define SENSORID1 true // true para sensor 1, false para sensor 2
#define SCD4 true // true para sensor SCD40
#define AS72 true // true para sensor AS7265X
#define BME true // true para sensor BME680
#define INFLUX true // true para enviar datos a InfluxDB, false para no enviar datos

#if SCD4
#include "SCD40Sensor.h"
#endif
#if AS72
#include "AS7265xModule.h"
#endif
#if BME
#include "BME680Module.h"
#endif
#if INFLUX
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <InfluxDBModule.h>
#endif
#if TEL
#include "WifiModule.h"
#include "MqttModule.h"
#include "HttpModule.h"

#endif
//=======================================================================
#if TEL

#if LOCAL
const char *ssid = "Convergentes"; // Nombre de la red WiFi
const char *password = "RedesConvergentes*#"; // Contraseña de la red WiFi
const char* server = "172.16.20.94"; // IP local del servidor MQTT
const char* server2 = "172.16.20.115"; // IP local del servidor HTTP
const int mqtt_port = 1883;
const int http_port = 3000;
#else
const char *ssid = "Familia Morales"; // Nombre de la red WiFi
const char *password = "2205631700"; // Contraseña de la red WiFi
const char* server = "200.122.207.134"; // IP publica del servidor MQTT
const char* server2 = "192.168.1.112"; // IP local del servidor HTTP
const int http_port = 8311;
const int mqtt_port = 8310;
const int mqtt_port2 = 1883;
const int http_port2 = 3000;
#endif

WifiModule wifiModule(ssid, password);
HttpModule httpClient(server, http_port);
HttpModule httpClient2(server2, http_port2);

#if INFLUX
#define INFLUXDB_URL "http://192.168.1.112:8086"
#define INFLUXDB_TOKEN "0Bj1esp9j4XtElS_cbQSBE7Sqk9VGS3NVpimrwhj-zTxzTsjKoOaK_3F2HX1QVjFUyFbJJUzzVbJVYTgwjW7GQ=="
#define INFLUXDB_ORG "47212db92d0632e6"
#define INFLUXDB_BUCKET "canna-wheater"
#define INFLUXDB_BUCKET2 "canna-lights"
#define DEVICE "650dc7d640e0be7842fc4239"
#define TZ_INFO "UTC-5"
#define DEVICE_BME "651203198748ed5dd33b6d2e"
#define DEVICE_AS "655dd4a264d0cd6c1628e4b3"
InfluxDBClient Influxclient(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
InfluxDBClient Influxclient2(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET2, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
InfluxDBModule influxDBModule;
Point sensor("SCD40");
Point sensor1("AS7265X");
Point sensor2("BME680");
#endif

#endif
//=======================================================================
#if SENSORID1
const char* sensor_id = "650dc7d640e0be7842fc4239"; // ID del sensor SCD40_1
#else
const char* sensor_id = "65391fa4700d51b6d681b3c5"; // ID del sensor SCD40_2
#endif
#if AS72
const char* sensor_id2 = "655dd4a264d0cd6c1628e4b3"; // ID del sensor AS7265X
#endif
#if BME
const char* sensor_id3 = "651203198748ed5dd33b6d2e"; // ID del sensor BME680
#endif
#if SCD4
SCD40Sensor SCD40(sensor_id);
#endif
#if AS72
AS7265xModule module(sensor_id2);
#endif
#if BME
BME680Module bme680(sensor_id3);
#endif
//=======================================================================
const unsigned long interval = 30000; // Intervalo de tiempo en milisegundos (0.5 min)
unsigned long previousMillis = 0;
//=======================================================================
#if AS72
void sendDataAS7265XToInflux(){
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, module.buildJson());
  sensor1.clearFields();
  for (JsonPair kv : doc.as<JsonObject>()) {
    sensor1.addField(kv.key().c_str(), kv.value().as<float>());
  }
  Serial.println(sensor1.toLineProtocol());
  if (!Influxclient2.writePoint(sensor1)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(Influxclient2.getLastErrorMessage());
  }
  else{
      Serial.println("InfluxDB write success");
  }
}
#endif
#if BME
void sendDataBME680ToInflux(){
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, bme680.buildJson());
  sensor2.clearFields();
  for (JsonPair kv : doc.as<JsonObject>()) {
    sensor2.addField(kv.key().c_str(), kv.value().as<float>());
  }
  Serial.println(sensor2.toLineProtocol());
  if (!Influxclient.writePoint(sensor2)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(Influxclient.getLastErrorMessage());
  }
  else{
      Serial.println("InfluxDB write success");
  }
}
#endif
void setup()
{
  Watchdog.enable(180000);
  Serial.begin(115200);
  #if SCD4
  Wire.begin();
  SCD40.begin(Wire);
  #endif
  #if AS72
    module.begin();
    module.setGain("1X", 50);
  #endif
  #if BME
    bme680.begin();
  #endif
    #if TEL
  wifiModule.conectarWifi();
  // mqttModule.conectarMQTT();
  #endif
  #if INFLUX
  influxDBModule.checkInfluxDB(Influxclient);
  sensor.addTag("device", DEVICE);
  sensor.addTag("SSID", ssid);
  #if AS72
  sensor1.addTag("device", DEVICE_AS);
  sensor1.addTag("SSID", ssid);
  #endif
  #if BME
  sensor2.addTag("device", DEVICE_BME);
  sensor2.addTag("SSID", ssid);
  #endif
  #endif
}

void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    #if SCD4
    SCD40.read();
    #endif
    #if AS72
    module.Read();
    delay(1000);
    Serial.println(module.buildJson());
    #endif
    #if BME
    bme680.Read();
    delay(1000);
    Serial.println(bme680.buildJson());
    #endif
    #if TEL
    #if SCD4
    String jsonString = SCD40.buildJson();
    httpClient.enviarDatosHTTP(jsonString.c_str(), "scd40");
    Serial.println("Enviando datos a servidor externo");
    httpClient2.enviarDatosHTTP(jsonString.c_str(), "scd40");
    Serial.println("Enviando datos a servidor local");
    #endif
    #if AS72
    String jsonString2 = module.buildJson();
    httpClient.enviarDatosHTTP(jsonString2.c_str(), "as7265x");
    //httpClient2.enviarDatosHTTP(jsonString2.c_str(), "as7265x");
    #endif
    #if INFLUX
    #if SCD4
    const float* co2 = SCD40.getCo2();
    const float* temperature = SCD40.getTemperatura();
    const float* humidity = SCD40.getHumedad();
    const float* VPD = SCD40.getVPD();
    sensor.clearFields();
    sensor.addField("co2_value", *co2);
    sensor.addField("temperature_value", *temperature);
    sensor.addField("humidity_value", *humidity);
    sensor.addField("VPD_value", *VPD);
    Serial.println(sensor.toLineProtocol());
    if (!Influxclient.writePoint(sensor)) {
      Serial.print("InfluxDB write failed: ");
      Serial.println(Influxclient.getLastErrorMessage());
    }
    else{
        Serial.println("InfluxDB write success");
    }
    #endif
    #if AS72
    sendDataAS7265XToInflux();
    #endif
    #if BME
    sendDataBME680ToInflux();
    #endif
    #endif
    #endif
    

}
Watchdog.reset();
}