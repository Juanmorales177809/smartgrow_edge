#include "BME680Module.h"
#include "Zanshin_BME680.h"
#include <Wire.h>



BME680Module::BME680Module(const char* id)
{
    this->id = id;
    BME680_Class sensor;
    float temperatura;
    float humedad;
    float presion;
}

void BME680Module::begin()
{
    sensor.begin(I2C_STANDARD_MODE);
    sensor.setOversampling(TemperatureSensor, Oversample16);  // Use enumerated type values
    sensor.setOversampling(HumiditySensor, Oversample16);     // Use enumerated type values
    sensor.setOversampling(PressureSensor, Oversample16);     // Use enumerated type values
    sensor.setIIRFilter(IIR4);
    sensor.setGas(320, 150);
}

void BME680Module::Read()
{
    static int32_t  temp, humidity, pressure, gas;
    static float    alt;                            // Temporary variable
    sensor.getSensorData(temp, humidity, pressure, gas); 
    temp = temp / 100.0;
    humidity = humidity / 1000.0;
    pressure = pressure / 100.0;
    gas = gas / 1000.0;
    this ->temperatura= temp;
    this->humedad = humidity;
    this->presion = pressure;
    Serial.print("Temperatura: ");
    Serial.println(this->temperatura);
    Serial.print("Humedad: ");
    Serial.println(this->humedad);
    Serial.print("Presion: ");
    Serial.println(this->presion);


    
    
}
// float* getTemperatura()
// {
//     return &this->temperatura;
// }

// float* getHumedad()
// {
//     return &this->humedad;
// }

// float* getPresion()
// {
//     return &this->presion;
// }

// float* getGas()
// {
//     return &this->gases;
// }



void BME680Module::setTemperatura(float temperatura)
{
    this->temperatura = temperatura;
}

void BME680Module::setHumedad(float humedad)
{
    this->humedad = humedad;
}

void BME680Module::setPresion(float presion)
{
    this->presion = presion;
}


const String BME680Module::buildJson()
{
    jsonDocument.clear();
    jsonDocument["temperatura"] = this->temperatura;
    jsonDocument["humedad"] = this->humedad;
    jsonDocument["presion"] = this->presion;
    jsonDocument["sensor"] = this->id;
    String jsonString;
    serializeJson(jsonDocument, jsonString);
    return jsonString;
}