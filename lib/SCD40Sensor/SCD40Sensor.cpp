#include "SCD40Sensor.h"

SCD40Sensor::SCD40Sensor(const char* id)
{   
    this->id = id;
    mySensor.begin();
};

const float* SCD40Sensor::getTemperatura() const
{
    return &temperatura;
};

const float* SCD40Sensor::getHumedad() const
{
    return &humedad;
};

const float* SCD40Sensor::getVPD() const
{
    return &VPD;
};

const float* SCD40Sensor::getCo2() const
{
    return &co2;
};

const char* SCD40Sensor::getId() const
{
    return id;
};

void SCD40Sensor::read()
{
    mySensor.readMeasurement();
    this->temperatura = mySensor.getTemperature();
    this->humedad = mySensor.getHumidity();
    this->co2 = mySensor.getCO2();
    this->VPD = calcularVPD(temperatura, humedad);
}

const String SCD40Sensor::buildJson() {
    jsonDocument.clear();
    jsonDocument["co2"] = this->co2;
    jsonDocument["temperatura"] = this->temperatura;
    jsonDocument["humedad"] = this->humedad;
    jsonDocument["VPD"] = this->humedad;
    jsonDocument["sensor"] = this->id;
    String jsonString;
    serializeJson(jsonDocument, jsonString);
    return jsonString;
}

float SCD40Sensor::calcularVPD(float temperatura, float humedad){
  return 0.611 * exp((17.27 * temperatura) / (temperatura + 237.3)) - (humedad / 100) * 0.611 * exp((17.27 * temperatura) / (temperatura + 237.3)); 
}
