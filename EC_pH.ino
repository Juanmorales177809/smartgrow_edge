#include "Control.h"
#include "AtlasSerialSensor.h"
//#include "SensorEEPROM.h"
//#include "SerialCom.h"
#include "ph_grav.h"
#include "SimpleKalmanFilter.h"
//#include "measureDistance.h"
//#include "OneWire.h"
//#include "DallasTemperature.h"

#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <Adafruit_SleepyDog.h>

#define MINUTE 1000L * 60

//#define WHOAMI "PH"
//SensorEEPROM sensorEEPROM = SensorEEPROM(WHOAMI);

ControlConfig configuration = {
    A1,          // POT_PIN
    8,           // M_UP
    9,           // M_DN,
    200,         // M_UP_SPEED,
    200,         // M_DN_SPEED,
    0,           // ZERO_SPEED,
    1000,        // DROP_TIME,
    0.3,         // ERR_MARGIN,
    10 * MINUTE, // STABILIZATION_TIME,
    0.1,         // STABILIZATION_MARGIN
    5 * 10,      // MAX_DESIRED_MEASURE
    7 * 10       // MIN_DESIRED_MEASURE
};
Control phControl = Control(&configuration);

ControlConfig ecUpConfiguration = {
    0,           // POT_PIN
    2,           // M_UP
    0,           // M_DN,
    200,         // M_UP_SPEED,
    200,         // M_DN_SPEED,
    0,           // ZERO_SPEED,
    10000,       // DROP_TIME,
    300,         // ERR_MARGIN,
    10 * MINUTE, // STABILIZATION_TIME,
    100,         // STABILIZATION_MARGIN
    0,           // MAX_DESIRED_MEASURE 0 if POT_PIN=0
    0            // MIN_DESIRED_MEASURE 0 if POT_PIN=0
};
Control ecUpControl = Control(&ecUpConfiguration);

#define EC_RX 10
#define EC_TX 11
AtlasSerialSensor ecSensor = AtlasSerialSensor(EC_RX, EC_TX);
SimpleKalmanFilter simpleKalmanEc(2, 2, 0.01);

#define GRAV_PH_PIN A0
Gravity_pH phSensor = Gravity_pH(GRAV_PH_PIN);
SimpleKalmanFilter simpleKalmanPh(2, 2, 0.01);

//SerialCom serialCom = SerialCom(&sensorEEPROM, &phControl);

//#define TANK_LVL_CM 50
//#define LVL_TRG_PIN 5
//#define LVL_ECHO_PIN 6
//MeasureDistance* measureDistance = new MeasureDistance(LVL_TRG_PIN,LVL_ECHO_PIN);

//Sensor temperatura
//#define TEMPERATURE_PIN 3
//OneWire oneWireObject(TEMPERATURE_PIN);
//DallasTemperature sensorDS18B20(&oneWireObject);

//unsigned long lastMillis;
//unsigned int SERIAL_PERIOD = 1 * MINUTE;

const char* ssid = "smartgrow";
const char* password = "2205631700";

void setup() {
    phSensor.begin();
//    sensorDS18B20.begin();
//    serialCom.init();

    phControl.setManualMode(false);
    phControl.setSetPoint(5.7);
    phControl.setReadSetPointFromCMD(true);

    ecUpControl.setManualMode(false);
    ecUpControl.setSetPoint(3000);
    ecUpControl.setReadSetPointFromCMD(true);
//    lastMillis = millis();
//    pinMode(13, OUTPUT);
//    digitalWrite(13, HIGH);

//    serialCom.printTask("PH", "START", 1);
//    serialCom.printTask("EC", "START", 1);
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
    //http.begin("http://172.1.1.19:8000/scd40");
    http.begin("http://10.1.36.74:8000/scd40");
    http.addHeader("Content-Type", "application/json");
    Serial.println("[HTTP] POST...");
    
    float ecReading = ecSensor.getReading();
    float ecKalman = simpleKalmanEc.updateEstimate(ecReading);
    ecUpControl.setCurrent(ecKalman);
    float ecSetpoint = ecUpControl.getSetPoint();
    ecUpControl.calculateError();

    float phReading = phSensor.read_ph();
    float phKalman = simpleKalmanPh.updateEstimate(phReading);
    phControl.setCurrent(phKalman);
    float phSetpoint = phControl.getSetPoint();    
    phControl.calculateError();


//    serialCom.checkForCommand();
//    
//    if ((millis() - lastMillis) > SERIAL_PERIOD)
//    {
//        sensorDS18B20.requestTemperatures();
//        lastMillis = millis();
//        serialCom.printTask("EC", "READ", ecReading);
//        delay(20);
//        serialCom.printTask("PH", "READ", phReading, phSetpoint);
//        delay(20);
//        serialCom.printTask("PH", "KALMAN", phKalman);
//        delay(20);
//        serialCom.printTask("LVL", "READ", TANK_LVL_CM - measureDistance->takeMeasure());
//        delay(20);
//        serialCom.printTask("TEMP", "READ", sensorDS18B20.getTempCByIndex(0));
//    }
//
//    int going = phControl.doControl();
//    if (going != GOING_NONE) {
//        serialCom.printTask(
//            "PH",
//            "CONTROL",
//            1000,
//            0,
//            phControl.getControlText(going)
//        );
//    }
//
//    int goingEc = ecUpControl.doControl();
//    if (goingEc != GOING_NONE)
//    {
//        serialCom.printTask(
//            "EC",
//            "CONTROL",
//            10000,
//            0,
//            ecUpControl.getControlText(goingEc)
//        );
//    }
    String json = "{\"EC\":" + String(ecReading) + ",\"pH\":" + String(phReading) + ",\"EC_setpoint\":" + String(ecSetpoint) + ",\"pH_setpoint\":" + String(phSetpoint) + "}";
    Serial.println(json);
    int httpCode = http.POST(json);
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    http.end();
    delay(1000); 
}
