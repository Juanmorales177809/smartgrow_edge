#include <Arduino.h>
#include <Ezo_i2c.h> 
#include <Wire.h>    
#include <sequencer2.h> 
#include <Ezo_i2c_util.h> 
#include <PeristalticsModule.h>
#include <LiquidCrystal_I2C.h>    
#include <EmoticonDisplay.h>   
#include <ReadModule.h>
#include <ControlModule.h>


#include "ph_grav.h"

#define TEL true
#define LOCAL false
#define PERSIT false
#define ELEC true
#define PHMETER true
#define DISPLAYS true
#define PT1000 true
#define CONTROL true
#define INFLUX true
//===============================================================
#if TEL
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFiModule.h"
#include "MqttModule.h"
#include "HttpModule.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Preferences.h>
#if INFLUX
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#endif

//===============================================================
#if LOCAL
const char *ssid = "Convergentes"; // Nombre de la red WiFi
const char *password = "RedesConvergentes*#"; // Contraseña de la red WiFi
#else
const char *ssid = "Familia Morales"; // Nombre de la red WiFi
const char *password = "2205631700"; // Contraseña de la red WiFi
#endif
//===============================================================
#if LOCAL
const char* server = "192.168.1.112";
const int mqtt_port = 1883;
const int http_port = 3000;
#else
const char* server = "200.122.207.134";
const int mqtt_port = 8310;
const int http_port = 8311;
#endif
//===============================================================
WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);
String sensorstring = "smartgrow/sensores/";
const char* sensor_id = "651b3c1a60ccd1c529a301d5"; // ID del sensor
//===============================================================
const unsigned long interval = 60000; // Intervalo de tiempo en milisegundos (1 min)
unsigned long previousMillis = 0;
//===============================================================
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
Preferences preferences;
//===============================================================
#if INFLUX
#define INFLUXDB_URL "http://192.168.1.112:8086"
#define INFLUXDB_TOKEN "0Bj1esp9j4XtElS_cbQSBE7Sqk9VGS3NVpimrwhj-zTxzTsjKoOaK_3F2HX1QVjFUyFbJJUzzVbJVYTgwjW7GQ=="
#define INFLUXDB_ORG "47212db92d0632e6"
#define INFLUXDB_BUCKET "canna-hidro"
#define DEVICE "651b3c1a60ccd1c529a301d5"
#define TZ_INFO "UTC-5"
InfluxDBClient Influxclient(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
Point sensor("phec");
#endif
//===============================================================
#endif
//===============================================================
#define lcd_addr 0x27
#define lcd_cols 16
#define lcd_rows 2
LiquidCrystal_I2C lcd(lcd_addr, lcd_cols, lcd_rows);
EmoticonDisplay bytes;
//===============================================================
void step1();  
void step2();
//===============================================================
ReadModule readModule;
float ph;                 
float temp;                
float ec;
//===============================================================
#if CONTROL
ControlModule controlModule;
bool control_state =  false;
#endif
//===============================================================
uint8_t user_bytes_received = 0;                
const uint8_t bufferlen = 32;                   
char user_data[bufferlen];   
//===============================================================
Sequencer2 Seq(&step1, 1000, &step2, 300);
//===============================================================
byte Celsius[8] = {
0b00110,
0b01001,
0b01000,
0b01000,
0b01001,
0b00110,
0b00000,
0b00000
};
byte smiley[8] = {
0b00000,
0b00000,
0b01010,
0b00000,
0b00000,
0b10001,
0b01110,
0b00000
};

byte frownie[8] = {
0b00000,
0b00000,
0b01010,
0b00000,
0b00000,
0b00000,
0b01110,
0b10001
};

byte email[8] = { 
                    0b11111,
                    0b00000,
                    0b01110,
                    0b00000,
                    0b00100,
                    0b00000,
                    0b00100,
                    0b00000
};
byte wifi[8] = {
0b00000,
0b00000,
0b00000,
0b00000,
0b00000,
0b00000,
0b00000,
0b11111
};
//===============================================================
void calibrate(){
  if (Serial.available() > 0) {                                                      
    user_bytes_received = Serial.readBytesUntil(13, user_data, sizeof(user_data));   
  }

   if (user_bytes_received) {                                                      
    readModule.parse_cmd(user_data);                                                          
    user_bytes_received = 0;                                                        
    memset(user_data, 0, sizeof(user_data));                                         
  }
  delay(1000);
}
void handleRecirculationTopic(){
  MqttModule::enviarMensajeMQTT(mqttClient, "recirculacion_hidroponico","smartgrow/hidroponico/actuadores");
}
void handleWaterInletTopic(){
  MqttModule::enviarMensajeMQTT(mqttClient, "entrada_de_agua_hidroponico","smartgrow/hidroponico/actuadores");
}
void handleWaterOutletTopic(){
  MqttModule::enviarMensajeMQTT(mqttClient, "desague_hidroponico","smartgrow/hidroponico/actuadores");
}
//===============================================================

#if TEL

void send_data(){
  // Verificar si la conexión con el servidor MQTT está activa
  if (!mqttClient.connected()) {//
    lcd.setCursor(15,1);
    lcd.write(byte(2));
    lcd.setCursor(0,1);
    MqttModule::conectarMQTT(mqttClient, server, mqtt_port);
  }else{
    lcd.setCursor(15,1);
    lcd.write(byte(1));
  }
  // Verificar si la conexión con el servidor HTTP está activa
  mqttClient.loop();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    StaticJsonDocument<200> jsonDocument; 
    jsonDocument["ph"] = ph;
    jsonDocument["ec"] = ec;
    jsonDocument["temperatura"] = temp;
    jsonDocument["sensor"] = sensor_id;

    // Serializar el JSON en una cadena
    String jsonString;
    serializeJson(jsonDocument, jsonString);
    //Serial.println(jsonString);
    lcd.setCursor(14,1);
    lcd.write(byte(5));
    HttpModule::enviarDatosHTTP(server, http_port, jsonString.c_str(), "phec");
    MqttModule::enviarMensajeMQTT(mqttClient, jsonString,"smartgrow/sensores/phec");
    
    delay(1000);
  }
  }
  #if INFLUX
  void checkInfluxDB(){
  // Verificar si la conexión con el servidor MQTT está activa
  if (!mqttClient.connected()) {//
    lcd.setCursor(15,1);
    lcd.write(byte(2));
    lcd.setCursor(0,1);
    MqttModule::conectarMQTT(mqttClient, server, mqtt_port);
  }
  }
  void send_data_influx(){
    sensor.clearFields();
    sensor.addField("ph", ph);
    sensor.addField("ec", ec);
    sensor.addField("temperatura", temp);
    //Serial.println(sensor.toLineProtocol());
    if (!Influxclient.writePoint(sensor)) {
      Serial.print("InfluxDB write failed: ");
      Serial.println(Influxclient.getLastErrorMessage());
    }else{
      Serial.println("InfluxDB write success");
    }
  }
  #endif
//===============================================================  
  #endif
//===============================================================
  void performTask() {
  // Tu tarea a realizar
  handleRecirculationTopic();
  Serial.println("Recirculando el sistema");
  delay(30000);
  handleRecirculationTopic();
  Serial.println("Apagando el sistema");
  delay(30000);
}
//===============================================================
void recirculateWaterDaily(){
  timeClient.update();
  long currentTime = timeClient.getEpochTime();
  long lastExecuted = preferences.getLong("lastExecuted", 0);

  if (currentTime - lastExecuted >= 840000) {
    performTask();
    preferences.putLong("lastExecuted", currentTime);
    Serial.println("Recirculando el sistema");
  }
}
//===============================================================
#if CONTROL
void control(void * parameters){
  
  for(;;){
    
      
    Serial.println("Setpoint EC: " + String(controlModule.setpoint_ec));
    if (controlModule.state_ec == false) {
        Serial.println("Control de EC iniciado");
        controlModule.set_ec(ec);
        controlModule.set_ph(ph);
        controlModule.set_tmp(temp);
        controlModule.control_ec();
        delay(90000);
      }else if (controlModule.state_ec == true && controlModule.state_ph == false) {
        controlModule.set_ph(ph);
        controlModule.control_ph();
        delay(600000);
      }else if (controlModule.state_ec == true && controlModule.state_ph == true) {
        Serial.println("Control de EC y PH finalizado");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.println("Control completo");
        
      }
    }
    
    }

    #endif
//===============================================================
void setup()
{
  //=================================================================
  Serial.begin(115200);
  Wire.begin();
  Seq.reset();
  delay(3000);
  #if CONTROL
  xTaskCreate(control, "control", 10000, NULL, 1, NULL);
  
  #endif
//===============================================================
  readModule.tds_activate();
  //=================================================================
  lcd.init();
  lcd.backlight();
  //=================================================================
  timeClient.setTimeOffset(-5 * 3600);
  preferences.begin("my-app", false);
  //=================================================================
  readModule.verifica_ph();
  //=================================================================
  #if CONTROL
  controlModule.setpoint_ph = 6.0;
  controlModule.setpoint_ec = 1000;
  controlModule.state_ec = false;
  #endif
  //=================================================================
  #if TEL
  sensorstring.reserve(30);  
  WiFiModule::conectarWiFi(ssid, password);
  mqttClient.setServer(server, mqtt_port);
  mqttClient.setCallback(MqttModule::callback);  
  #endif
  timeClient.begin();
  preferences.begin("my-app", false);

  // Obtener la última vez que se ejecutó la tarea
  long lastExecuted = preferences.getLong("lastExecuted", 0);
  long currentTime = timeClient.getEpochTime();

  // Verificar si han pasado 24 horas
  if (currentTime - lastExecuted >= 60) { // 86400 segundos en un día
    performTask();
    preferences.putLong("lastExecuted", currentTime);
  }
  #if INFLUX
  checkInfluxDB();
  sensor.addTag("device", DEVICE);
  sensor.addTag("SSID", ssid);
  #endif
}
//===============================================================


void loop()
{
  Seq.run();
}

void step1(){
  readModule.send_a_read_temp();
  readModule.send_a_read_ec();
  ph = readModule.receive_and_print_reading_pH();
//===============================================================
  lcd.createChar(0, Celsius);
  lcd.createChar(1, smiley);
  lcd.createChar(2, frownie);
  lcd.createChar(3, email);
  lcd.createChar(4, wifi);
  lcd.setCursor(0,0);
//===============================================================  
  calibrate();
  //=================================================================
  recirculateWaterDaily();
}

void step2(){
    calibrate();
    temp = readModule.receive_and_print_reading_Temp();   
    ec = readModule.receive_and_print_reading_EC();

    #if TEL
    send_data(); // enviar datos para el backend
    #endif
    #if INFLUX
    send_data_influx();
    #endif
    lcd.setCursor(0,0);
    lcd.print("PH: ");
    lcd.print(ph);
    lcd.setCursor(8,0);
    lcd.print(" ");
    lcd.setCursor(0,1);
    lcd.print("EC: ");
    lcd.print(ec);
    
    // Serial.println();
    // Serial.print("PH: ");
    // Serial.println(ph);
    // Serial.print("EC: ");
    // Serial.println(ec);
  
    lcd.setCursor(9,0);
    lcd.print("T:");
    lcd.setCursor(11,0);
    lcd.print(temp);
    lcd.setCursor(15,0);
    lcd.write(byte(0));
    lcd.setCursor(15,1);
    lcd.write(byte(1));
    
    // #if CONTROL
    // control_state = true;
    // #endif
    //===============================================================
}