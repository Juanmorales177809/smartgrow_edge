#ifndef BME680Module_h
#define BME680Module_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include "Zanshin_BME680.h"


class BME680Module {
    public:
        BME680Module(const char* id);
        float temperatura;
        float humedad;
        float presion;
        void begin();
        void Read();
        const String buildJson();
        // const float* getTemperatura() const;
        // const float* getHumedad() const;
        // const float* getPresion() const;
        // const float* getGas() const;
        void setTemperatura(float temperatura);
        void setHumedad(float humedad);
        void setPresion(float presion);
        const char* id;
        static int32_t  temp, humidity, pressure, gas;
        BME680_Class sensor;
        StaticJsonDocument<200> jsonDocument;
};
#endif