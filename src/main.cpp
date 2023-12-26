#include <Arduino.h>
#include <Ezo_i2c.h> 
#include <Wire.h>    
#include <sequencer2.h> 
#include <Ezo_i2c_util.h> 
#include <PeristalticsModule.h>
#include <LiquidCrystal_I2C.h>    
#include <EmoticonDisplay.h>    

#include "ph_grav.h"

#define TEL true
#define LOCAL false
#define PERSIT false
#define ELEC true
#define PHMETER true
#define DISPLAYS true
#define PT1000 true

#if TEL
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFiModule.h"
#include "MqttModule.h"
#include "HttpModule.h"

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
const char* server = "172.16.20.94";
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
const unsigned long interval = 1000; // Intervalo de tiempo en milisegundos (1 min)
unsigned long previousMillis = 0;
#endif
//===============================================================
#if PHMETER
Gravity_pH pH = Gravity_pH(32);
#endif
#if ELEC
Ezo_board EC = Ezo_board(100, "EC");
#endif
#if PERSIT
PeristalticsModule peristalticsModule; 
#endif
//===============================================================
#if PT1000
Ezo_board RTD = Ezo_board(102, "RTD"); 
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
#if ELEC
char EC_data[32];
char *EC_str;                     //char pointer used in string parsing.
char *TDS;                       //char pointer used in string parsing.
float EC_float;
float TDS_float;
#endif
#if PHMETER
float ph;                 
float temp_float;                
float analog_ph;
float voltage;
#endif
#if PT1000
char computerdata[20];
char RTD_data[20]; 
float tmp_float; 
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


void setup()
{
  //=================================================================
  Serial.begin(115200);
  Wire.begin();
  Seq.reset();
  delay(3000);
  #if ELEC
  EC.send_cmd("o,tds,1"); 
  delay(3000);
  #endif
  
  lcd.init();
  lcd.backlight();
  
  
  #if PHMETER
  if (pH.begin()) {                                     
    Serial.println("Loaded EEPROM");
  }
  else {
    Serial.println("Error loading EEPROM");
  }
  #endif
  #if PERSIT
  peristalticsModule.configInit();
  #endif

  #if TEL
  sensorstring.reserve(30);  
  WiFiModule::conectarWiFi(ssid, password);
  mqttClient.setServer(server, mqtt_port);
  mqttClient.setCallback(MqttModule::callback);  
  #endif
}

#if ELEC
void send_a_read(){
  EC.send_cmd("r");
  //EC.send_cmd_with_num("T,", 21.0);
  
}
void send_a_read_step2(){
  EC.receive_cmd(EC_data, 32);
  EC_str = strtok(EC_data, ",");       //let's parse the string at each comma.
  TDS = strtok(NULL, ",");
  EC_float=atof(EC_str);               //convert char to float.
  TDS_float=atof(TDS);              //convert char to float.
}
#endif
#if PT1000
void receive_and_print_reading_Temp(){
  RTD.receive_cmd(RTD_data, 20); 
  tmp_float = atof(RTD_data); 
  Serial.print("Temperature: ");
  Serial.print(tmp_float);
  Serial.println(" C");
}
#endif
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
    jsonDocument["ec"] = EC_float;
    jsonDocument["temperatura"] = tmp_float;
    jsonDocument["sensor"] = sensor_id;

    // Serializar el JSON en una cadena
    String jsonString;
    serializeJson(jsonDocument, jsonString);
    Serial.println(jsonString);
    lcd.setCursor(14,1);
    lcd.write(byte(3));
    HttpModule::enviarDatosHTTP(server, http_port, jsonString.c_str(), "phec");
    MqttModule::enviarMensajeMQTT(mqttClient, jsonString);
    delay(1000);
  }
  }
  
  #endif
  

  


void loop()
{
  Seq.run();
  

}

void parse_cmd(char* string) {     
  #if PHMETER              
  strupr(string);                                
  if (strcmp(string, "CAL,7") == 0) {       
    pH.cal_mid();                                
    Serial.println("MID CALIBRATED");
  }
  else if (strcmp(string, "CAL,4") == 0) {            
    pH.cal_low();                                
    Serial.println("LOW CALIBRATED");
  }
  else if (strcmp(string, "CAL,10") == 0) {      
    pH.cal_high();                               
    Serial.println("HIGH CALIBRATED");
  }
  else if (strcmp(string, "CAL,CLEAR") == 0) { 
    pH.cal_clear();                              
    Serial.println("CALIBRATION CLEARED");
  }
  #endif
}

void step1(){
  #if PT1000
  RTD.send_read_cmd();
  #endif
  #if ELEC
  send_a_read();
  #endif

  
  lcd.createChar(0, Celsius);
  lcd.createChar(1, smiley);
  lcd.createChar(2, frownie);
  lcd.createChar(3, email);
  lcd.setCursor(0,0);
  
  //EC.send_cmd_with_num("T,", 25.0);
  #if PHMETER
  voltage = pH.read_voltage();
  ph = pH.read_ph(voltage);                      
  analog_ph = float(ph);
  #endif
  if (Serial.available() > 0) {                                                      
    user_bytes_received = Serial.readBytesUntil(13, user_data, sizeof(user_data));   
  }

   if (user_bytes_received) {                                                      
    parse_cmd(user_data);                                                          
    user_bytes_received = 0;                                                        
    memset(user_data, 0, sizeof(user_data));                                         
  }
  delay(1000);
}

void step2(){
  #if PT1000
  receive_and_print_reading_Temp();
  #endif
  #if PHMETER
  Serial.print("PH: ");
  Serial.println(ph);
  #endif
  #if ELEC
  send_a_read_step2();
  //receive_and_print_reading(EC);             //get the reading from the EC circuit
  #endif
    #if TEL
  send_data(); // enviar datos para el backend
  #endif
  #if PERSIT
  if (peristalticsModule.estado == false){
  peristalticsModule.acciones("bionovaA", true);
  }else{
    peristalticsModule.acciones("bionovaA", false);
  }
  // if (peristalticsModule.estado == true)
  // peristalticsModule.acciones("bionovaA", false);
  // peristalticsModule.acciones("bionovaA", false);
  // delay(10000);
  // peristalticsModule.acciones("bionovaB",false);
  // peristalticsModule.acciones("phDown", false);
  #endif
  lcd.setCursor(0,0);
  lcd.print("PH: ");
  lcd.print(ph);
  lcd.setCursor(0,1);
  lcd.print("EC: ");
  lcd.print(EC_float);
  
  Serial.println();
  #if ELEC
  Serial.print("EC: ");
  Serial.println(EC_float);
  Serial.print("TDS: ");
  Serial.println(TDS_float);
  #endif
  
  lcd.setCursor(9,0);
  lcd.print("T:");
  lcd.setCursor(11,0);
  lcd.print(tmp_float);
  lcd.setCursor(15,0);
  lcd.write(byte(0));
  lcd.setCursor(15,1);
  lcd.write(byte(1));
  
  
  
}