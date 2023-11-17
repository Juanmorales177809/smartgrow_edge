#ifndef SCD40Sensor_h
#define SCD40Sensor_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "SparkFun_SCD4x_Arduino_Library.h"

class SCD40Sensor {
    public:
        SCD40Sensor(const char* id);
        const float* getTemperatura() const;
        const float* getHumedad() const;
        const float* getCo2() const;
        const float* getVPD() const;
        const char* getId() const;
        void read();
        const String buildJson();
    private:
        const char* id;
        float temperatura;
        float humedad;
        float co2;
        float VPD;
        float calcularVPD(float temperatura, float humedad);
        SCD4x mySensor;
        StaticJsonDocument<200> jsonDocument;
};

#endif