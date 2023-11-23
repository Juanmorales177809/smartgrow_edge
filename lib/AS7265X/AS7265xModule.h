#ifndef AS7265xModule_h
#define AS7265xModule_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include "SparkFun_AS7265X.h"


class AS7265xModule {
public:
    AS7265xModule(const char* id);
    void Read();
    void begin(const char* gain, const int integration);
    const String buildJson();
private:
    const char* id;
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    float G;
    float H;
    float R;
    float I;
    float S;
    float J;
    float T;
    float U;
    float V;
    float W;
    float K;
    float L;
    AS7265X sensor;
    StaticJsonDocument<200> jsonDocument;
    
    
};

#endif