#include "SCD40Sensor.h"
#include <SensirionI2CScd4x.h>
#include <Wire.h>


void SCD40Sensor::read()
{
    uint16_t co2 = 0;
    float temperature = 0.0f;
    float humidity = 0.0f;
    mySensor.readMeasurement(co2, temperature, humidity);
    this->temperatura = temperature;
    this->humedad = humidity;
    this->co2 = co2;
    this->VPD = calcularVPD(temperature, humidity);
    Serial.print("Temperatura: ");
    Serial.println(this->temperatura);
    Serial.print("Humedad: ");
    Serial.println(this->humedad);
    Serial.print("CO2: ");
    Serial.println(this->co2);
    Serial.print("VPD: ");
    Serial.println(this->VPD);
}

const String SCD40Sensor::buildJson()
{
    jsonDocument.clear();
    jsonDocument["co2"] = this->co2;
    jsonDocument["temperatura"] = this->temperatura;
    jsonDocument["humedad"] = this->humedad;
    jsonDocument["VPD"] = this->calcularVPD(temperatura, humedad);
    jsonDocument["sensor"] = this->id;
    String jsonString;
    serializeJson(jsonDocument, jsonString);
    return jsonString;
}

float SCD40Sensor::calcularVPD(float temperatura, float humedad)
{
    if (temperatura == 0.0f || humedad == 0.0f)
    {
        return 0.0f;
    }
    VPD = 0.611 * exp((17.27 * temperatura) / (temperatura + 237.3)) * (1-(humedad/100)); //Calculo de VPD  con la formula de Buck 1996 
    
    return VPD;

}

SCD40Sensor::SCD40Sensor(const char* id)
{
    this->id = id;
}

void SCD40Sensor::begin(TwoWire &wire)
{
    mySensor.begin(wire);
    mySensor.startPeriodicMeasurement();
}

void SCD40Sensor::setTemperatura(float temperatura)
{
    this->temperatura = temperatura;
}

void SCD40Sensor::setHumedad(float humedad)
{
    this->humedad = humedad;
}

void SCD40Sensor::setVPD(float VPD)
{
    this->VPD = VPD;
}

void SCD40Sensor::setCo2(float co2)
{
    this->co2 = co2;
}

const float* SCD40Sensor::getTemperatura() const
{
    return &temperatura;
}

const float* SCD40Sensor::getHumedad() const
{
    return &humedad;
}

const float* SCD40Sensor::getVPD() const
{
    return &VPD;
}

const float* SCD40Sensor::getCo2() const
{
    return &co2;
}

const char* SCD40Sensor::getId() const
{
    return id;
}


